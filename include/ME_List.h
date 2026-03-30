#ifndef __ME_LIST_H
#define __ME_LIST_H

#include "ME_File.h"

#include <stddef.h>

void me_try_list(ME_File* file);
void me_get_list_info_fields(ME_File* file);
int me_remove_list_info_field(ME_File* file, size_t field_index);

//void me_write_list_info_chunk(ME_File* file, ME_Array field_arr);
//void me_write_list_id3v2_chunk(ME_File* file, ME_Array field_arr);

#endif
