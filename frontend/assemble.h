#ifndef ASSEMBLE
#define ASSEMBLE

// TODO: Make struct containing information about the overall
// 	 assembling.
static uint8_t		*bin_data;
static size_t		bind = 0;
static size_t		_udind = 0;

void update_bin()
{
	bind++;
	bin_data = realloc(
			bin_data,
			(bind+1) * sizeof(*bin_data)
			);
}

void assemble(PBlock_t **blocks, size_t size)
{
	bin_data = calloc(1, sizeof(*bin_data));
	for(size_t i = 0; i < size; i++)
	{
		if(blocks[i]->binary_info)
		{
			if(blocks[i]->curr_tblock->token_id != user_defined)
			{
				bin_data[bind] = blocks[i]->binary_info->opcode;

				update_bin();
			}

			switch(blocks[i]->curr_tblock->token_id)
			{
				case r_ax:
				{
					bin_data[bind] = blocks[i]->binary_info->reg_info->parent_val & 0xFF;
			
					update_bin();

					short s = (blocks[i]->binary_info->reg_info->parent_val >> 8) & 0xFF;
					memcpy(&bin_data[bind], &s, sizeof(short));
			
					update_bin();
					break;
				}
				case r_ah:
				{
					bin_data[bind] = blocks[i]->binary_info->reg_info->high_bit_val;
					update_bin();

					break;
				};
				case r_al:
				{
					bin_data[bind] = blocks[i]->binary_info->reg_info->low_bit_val;
					update_bin();

					break;
				}
				case user_defined:
				{
					for(size_t i = 0; i < strlen(_user_defined[_udind]->db_value); i++)
					{
						bin_data[bind] = _user_defined[_udind]->db_value[i];
						update_bin();
					}
					//printf("%c", bin_data[0]);
					//update_bin();		
					break;
				}
				default:break;
			}
		}
	}

	FILE* file = fopen("a.bin", "wb");

	fwrite(bin_data, bind, 1, file);
	fclose(file);	
}

#endif
