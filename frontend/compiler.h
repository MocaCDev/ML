#ifndef COMPILER
#define COMPILER
#include "common.h"
#include "assemble.h"

typedef struct Compiler
{
	PBlock_t	**blocks;
	PBlock_t	*curr_block;
	size_t		size;
	size_t		index;
} Compiler_t;

#define token_id(c)	c->curr_block->curr_tblock->token_id
#define token_val(c)	c->curr_block->curr_tblock->token_value
#define bi(c)		c->curr_block->binary_info

void next_block(Compiler_t *compiler)
{
	compiler->index++;
	compiler->curr_block = compiler->blocks[compiler->index];
}

void handle_mov(Compiler_t *compiler)
{
	switch(token_id(compiler))
	{
		case r_ax:
		{
			PBlock_t *block = compiler->curr_block;
			next_block(compiler);

			uint16_t value = 0;

			if(token_id(compiler) == hex)
				value = conv_from_hex(token_val(compiler));
			else if(token_id(compiler) == dec)
				value = atoi(token_val(compiler));
			else
				err("Failed");	

			RegInfo_t last_r;
			last_r = *ax_reg;

			assign_reg_value(ax_reg, value);

			block->binary_info->opcode = mov_ax;
			block->binary_info->reg_info = ax_reg;

			break;
		}
		case r_ah:
		{
			PBlock_t *block = compiler->curr_block;
			next_block(compiler);

			uint8_t value = 0;
			if(token_id(compiler) == hex)
				value = conv_from_hex(token_val(compiler));
			else if(token_id(compiler) == dec)
				value = atoi(token_val(compiler));
			else
				err("Failed");
		
			block->binary_info->opcode = mov_ah;
			block->binary_info->reg_info = calloc(1, sizeof(*block->binary_info->reg_info));
			block->binary_info->reg_info->high_bit_val = value;

			break;
		}
		case r_al:
		{
			PBlock_t *block = compiler->curr_block;
			next_block(compiler);

			uint8_t value = 0;
			if(token_id(compiler) == hex)
				value = conv_from_hex(token_val(compiler));
			else if(token_id(compiler) == dec)
				value = atoi(token_val(compiler));
			else
				err("Failed");
			//assign_child_value(ax_reg, 1, value);

			block->binary_info->opcode = mov_al;
			block->binary_info->reg_info = calloc(1, sizeof(*block->binary_info->reg_info));
			block->binary_info->reg_info->low_bit_val = value;
			
			break;
		}
		default: break;
	}
}

void handle_user_defined(Compiler_t *compiler)
{
	switch(token_id(compiler))
	{
		case Tdb:
		{
			_user_defined[udind]->type = db;

			next_block(compiler);

			if(
				token_id(compiler) != hex && token_id(compiler) != dec  &&
				token_id(compiler) != quote)
				err("Expected a value assignment")
			
			_user_defined[udind]->db_value = token_val(compiler);

			udind++;
			_user_defined = realloc(
					  _user_defined,
					  (udind+1)*sizeof(*_user_defined)
					);
			_user_defined[udind] = calloc(1, sizeof(*_user_defined[udind]));
			break;
		}
		default: break;
	}
}

void compile(PBlock_t **blocks, size_t size)
{
	Compiler_t *compiler	= calloc(1, sizeof(*compiler));
	compiler->blocks	= blocks;
	compiler->size		= size;
	compiler->index		= 0;
	compiler->curr_block	= compiler->blocks[compiler->index];

	do {
		switch(compiler->curr_block->curr_tblock->token_id)
		{
			case mov:
			{
				next_block(compiler);

				handle_mov(compiler);

				break;				
			}
			case user_defined:
			{
				next_block(compiler);

				handle_user_defined(compiler);

				break;		
			}
			case eof: goto end;
			default: break;
		}	
		next_block(compiler);
	} while(compiler->index < compiler->size);

end:
	assemble(compiler->blocks, compiler->size);
}

#endif
