/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ft2build.h>
#include<stdint.h>
#include FT_FREETYPE_H
#include <vector>
#include <iostream>
#include <map>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <iterator>
#include <fstream>
#include "utf8.h"
#include <filesystem>



short resolution;
int size;
std::string font;
int base;
int NbChar;

std::vector<unsigned char> intToByteArray(int k){
	std::vector<unsigned char> res(4);
	res[0] =  k & 0x000000ff;
	res[1] = (k & 0x0000ff00) >> 8;
	res[2] = (k & 0x00ff0000) >> 16;
	res[3] = (k) >> 24;
	return res;
}
std::vector<unsigned char> shortToByteArray(short k){
	std::vector<unsigned char> res(2);
	res[0] =  k & 0x000000ff;
	res[1] = (k & 0x0000ff00) >> 8;
	
	return res;
}


void draw_bitmap( FT_Bitmap*  bitmap, FT_Int x, FT_Int y, std::vector<uint8_t> &buffer_letter, const int baseline)
{
	int i,j,p,q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;
	int origin = y;
	int lowest_y =  bitmap->rows; 
	int highest_y = 0;
	uint8_t column_number = bitmap->width;
	uint8_t line_number = bitmap->rows;
	int baseline_x = 0xFF;
	uint8_t offset_x = baseline_x-x;
	uint8_t byteoffy1,byteoffy2;
	 
	int offset_y = baseline-y;
	byteoffy1 = (offset_y & 0xFF00)>>8;
	byteoffy2 = offset_y & 0xFF;
	

	uint8_t byteArray[] = {column_number,0,line_number,0,byteoffy2,byteoffy1,0x0,0x0,0x0,0,0x01,0x0};
	 

	for (int h=0;h<12;h++) buffer_letter.push_back(byteArray[h]);
	uint8_t cur_pix_to_render = 0x0;
	int activate = 0;
	
	for (int q = 0; q < line_number; q++ )
	{
		for (p = 0; p < column_number; p++ )
		{
			
			activate++;
			if (activate==2){
	
				activate = 0;
				cur_pix_to_render = cur_pix_to_render + ((bitmap->buffer[q * bitmap->width + p])& 0xF0);
				
				uint8_t right = ((cur_pix_to_render & 0xF0));
				buffer_letter.push_back(cur_pix_to_render);
				
			}
			else {
				cur_pix_to_render = ((bitmap->buffer[q * bitmap->width + p])&0xF0)>>4;
				
			}
		}
		
	  }
	  if (activate == 1){
		  buffer_letter.push_back(cur_pix_to_render);
	  }
	
}

std::vector<uint8_t> draw_character(int character_code, int *length, FT_Library library, FT_Face &face, FT_GlyphSlot  slot, const int baseline){
	std::vector<uint8_t> result = std::vector<uint8_t>();

    FT_Load_Glyph( face, character_code, FT_LOAD_RENDER );
	FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
    

    draw_bitmap( &slot->bitmap,
                 slot->bitmap_left,
                 slot->bitmap_top,
				 result,
				 baseline);

	
	
	return result;
  
   
}

struct character {
  int addr;
  int code;
  
  character(int a, int c){
	  addr = a;
	  code = c;
	  
  }
  bool operator<(const character& rhs) const
    {
        if (addr < rhs.addr)
        {
           return true;
        }
        else return false;
    }
	bool operator==(const character& rhs) const
    {
        if (code == rhs.code)
        {
           return true;
        }
        else return false;
    }
  
};


void create_node(std::vector<character> &characters, std::vector<int> &pile_of_additional_characters, int code, int idx, int up, int low){ //each node, creates two nodes, left has lower code and right has higher,
	
	int low_idx = (low+code)/2;	
	int high_idx = (up+code)/2;	
	
	bool isAlreadyThere = std::find(characters.begin(), characters.end(), character(idx, code)) != characters.end();
	
	if (!isAlreadyThere) characters.push_back(character(idx, pile_of_additional_characters[code]));
	else characters.push_back(character(idx, -1));
	
	if (low_idx == high_idx) {
		
		if (pile_of_additional_characters[low_idx] > pile_of_additional_characters[code])
			characters.push_back(character(2*idx+1, pile_of_additional_characters[low_idx]));
			
		else 
			characters.push_back(character(2*idx, pile_of_additional_characters[low_idx]));
		
		return;
	}
	
		if (pile_of_additional_characters[low_idx] < pile_of_additional_characters[code]){
			create_node(characters, pile_of_additional_characters, low_idx, 2*idx, code, low);
		}

		if (pile_of_additional_characters[high_idx] > pile_of_additional_characters[code]){
			
			create_node(characters, pile_of_additional_characters, high_idx, 2*idx+1, up, code);
		}
	
}
int main( int     argc,
      char**  argv )
{
	
	std::ifstream file("config_font.ini");
	std::string str;
	std::size_t idx = -1;
	int code1 = 0;
	std::vector<int> pile_of_additional_characters;
	std::cout << "Reading config file..." << std::endl;
	while(getline(file, str)) 
    {
			if (idx=str.find("FontSize")!=-1) {
			std::string size_str = str.substr(str.find("=")+1,str.length()-1);
			size = std::stoi(size_str);
			}
			else if (idx=str.find("Resolution")!=-1) {	 resolution = (uint8_t) std::stoi(str.substr(str.find("=")+1,str.length()));}
			else if (idx=str.find("Font")!=-1) font = str.substr(str.find("=")+1,str.length());
			else if (idx=str.find("Base")!=-1) base = std::stoi(str.substr(str.find("=")+1,str.length()));
			else if (idx=str.find("NbChar")!=-1) NbChar = std::stoi(str.substr(str.find("=")+1,str.length()));
			
    }
	std::cout << "Resolution " << resolution << std::endl;
	std::cout << "Font " << font << std::endl;
	std::cout << "Base " << base << std::endl;
	std::cout << "NbChar " << NbChar << std::endl;
	std::cout << "FontSize " << size << std::endl;
	file.close();
	for (int i = 0; i<NbChar; i++){
		pile_of_additional_characters.push_back(i);
	}	
	
	std::sort(pile_of_additional_characters.begin(), pile_of_additional_characters.end());
	
	
	std::vector<unsigned char> buffer_font_file;
	std::vector<unsigned char> resolution_bytes = shortToByteArray(resolution);
	
	int idk_what_that_is = 0x0D;
	std::vector<unsigned char> nb_char_bytes = intToByteArray(pile_of_additional_characters.size());
	std::vector<unsigned char> bytes_idk = intToByteArray(idk_what_that_is);
	buffer_font_file.push_back(0x01);
	buffer_font_file.push_back(0x01);
	buffer_font_file.insert(buffer_font_file.end(), resolution_bytes.begin(),resolution_bytes.end());
	
	buffer_font_file.insert(buffer_font_file.end(),nb_char_bytes.begin(), nb_char_bytes.end());
	buffer_font_file.insert(buffer_font_file.end(),nb_char_bytes.begin(), nb_char_bytes.end());
	buffer_font_file.insert(buffer_font_file.end(),bytes_idk.begin(), bytes_idk.end());
	
	for (int i = 0; i<idk_what_that_is-1; i++){
		std::vector<unsigned char> zero_bytes =intToByteArray(0);
		buffer_font_file.insert(buffer_font_file.end(),zero_bytes.begin(),zero_bytes.end());
	} 
	
	
	std::vector<character> characters;	
	
	int last_addr = 0;  
	int pos_of_declaration_in_file;
	
	
	int first_code = pile_of_additional_characters.size()/2;
	idx = 0;
	pos_of_declaration_in_file = 0xC + idk_what_that_is*4 + 0x8*idx;
	auto it_max = max_element(std::begin(pile_of_additional_characters), std::end(pile_of_additional_characters)); 
	auto it_min = min_element(std::begin(pile_of_additional_characters), std::end(pile_of_additional_characters)); 
	create_node(characters, pile_of_additional_characters, first_code, 1, it_max - pile_of_additional_characters.begin(), it_min - pile_of_additional_characters.begin());
	
	std::sort(characters.begin(), characters.end());
	
	FT_Library    library;
	FT_Face       face;
	FT_GlyphSlot  slot;
	FT_Vector     pen;                   
	FT_Error      error;
	
	int           n, num_chars;
	std::string filename = font;
	
	
	int glyph_code;
	int length = 0;
	
	error = FT_Init_FreeType( &library );             
	error = FT_New_Face( library, filename.c_str(), 0, &face );
	const int baseline = resolution * base/0x20;
	int actual_resolution = resolution * 530/0x40; 
	error = FT_Set_Char_Size(face, size, 0, actual_resolution, 0);                
	slot = face->glyph;
	int off_y;
	std::vector<character>::iterator it = characters.begin();
	int globalOffset = 0;
	int end_drawing;
	int end_code;
	const FT_Byte*  file_base;
	

	std::vector<unsigned char> addr_section, drawing_section;
	int current_position = buffer_font_file.size() + characters.size() * 8;
	for (it = characters.begin(); it != characters.end(); it++){
		
		character current_char = *it;
		
		slot = face->glyph;
		int char_code = current_char.code;
		
		glyph_code = FT_Get_Char_Index(face, char_code );
		
		if (glyph_code == 0){
			std::vector<unsigned char> code_bytes = intToByteArray(current_char.code);
			std::vector<unsigned char> addr_bytes = intToByteArray(buffer_font_file.size() + characters.size() * 8);
			addr_section.insert(addr_section.end(),code_bytes.begin(), code_bytes.end());
			addr_section.insert(addr_section.end(),addr_bytes.begin(),addr_bytes.end());
			
			
		} 
		else{ 
			
			std::vector<unsigned char> letter = draw_character(glyph_code,&length, library, face, slot, baseline);
			
			std::vector<unsigned char> code_bytes = intToByteArray(current_char.code);
			std::vector<unsigned char> addr_bytes = intToByteArray(current_position);
			addr_section.insert(addr_section.end(),code_bytes.begin(), code_bytes.end());
			addr_section.insert(addr_section.end(),addr_bytes.begin(),addr_bytes.end());
			
			
			current_position += letter.size();
			drawing_section.insert(drawing_section.end(),letter.begin(), letter.end());
			
		}
		  
	}
	buffer_font_file.insert(buffer_font_file.end(),addr_section.begin(), addr_section.end());
	buffer_font_file.insert(buffer_font_file.end(),drawing_section.begin(), drawing_section.end());
	error = FT_Done_FreeType( library );
	
	std::ofstream writeFontFile;
	writeFontFile.open("font_us.itf", std::ios::out | std::ios::binary);
	if ( writeFontFile ) {
	writeFontFile.write((const char*)&buffer_font_file[0], buffer_font_file.size());
	writeFontFile.close();
	std::cout << "SUCCESS!";
	}
	else {
	   std::cout << "FAIL!";
	   
	}
	scanf("%d");

	  return 0;
}

/* EOF */