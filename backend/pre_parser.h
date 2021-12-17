#ifndef PRE_PARSER
#define PRE_PARSER
#include <stdint.h>

typedef struct RegInfo
{
	uint16_t	parent_val;
	uint8_t		high_bit_val;
	uint8_t		low_bit_val;
} RegInfo_t;

typedef struct UserDefined
{
	char	*name;
	enum {
		db,
		dw,
		dd
	} type;

	uint8_t		*db_value;
	uint16_t	dw_value;
	uint32_t	dd_value;
} UserDefined_t;

static RegInfo_t	*ax_reg;
static RegInfo_t	*bx_reg;
static RegInfo_t	*cx_reg;
static RegInfo_t	*dx_reg;
static UserDefined_t	**_user_defined;
static size_t		udind = 0;

/*
 * BinaryInfo - Information about current instruction and it's opcode.
 * */
typedef struct BinaryInfo
{
	enum {
		mov_ax = 0xB8,
		mov_ah = 0xB4,
		mov_al = 0xB0,
		user_def = 0x0,
	} opcode;

	/*
	 * reg_info will be a pointer to one of the pre-defined RegInfo_t variables.
	 * It will store the information about the register at the current state of the
	 * program.
	 * */
	RegInfo_t	*reg_info;
} BinaryInfo_t;

void init_regs()
{
	_user_defined = calloc(udind+1, sizeof(*_user_defined));
	_user_defined[udind] = calloc(1, sizeof(*_user_defined[udind]));
	
	ax_reg = calloc(1, sizeof(*ax_reg));
	bx_reg = calloc(1, sizeof(*bx_reg));
	cx_reg = calloc(1, sizeof(*cx_reg));
	dx_reg = calloc(1, sizeof(*dx_reg));

	ax_reg->parent_val = ax_reg->high_bit_val = ax_reg->low_bit_val = 0;
	bx_reg->parent_val = bx_reg->high_bit_val = bx_reg->low_bit_val = 0;
	cx_reg->parent_val = cx_reg->high_bit_val = cx_reg->low_bit_val = 0;
	dx_reg->parent_val = dx_reg->high_bit_val = dx_reg->low_bit_val = 0;
}

void assign_reg_value(RegInfo_t *reg_info, uint16_t buffer)
{
	reg_info->high_bit_val = buffer >> 8;
	reg_info->low_bit_val = buffer & ((1 << 8)-1);
	reg_info->parent_val = (reg_info->high_bit_val << 8) | reg_info->low_bit_val;
}

void assign_child_value(RegInfo_t *reg_info, uint8_t type, uint8_t buffer)
{
	if(type == 0) // ah
	{
		reg_info->high_bit_val = buffer;
		reg_info->low_bit_val = 0;

		reg_info->parent_val = (reg_info->high_bit_val) | reg_info->low_bit_val;
		return;
	}
	// al
	reg_info->low_bit_val = buffer;
	reg_info->high_bit_val = 0;
		
	reg_info->parent_val = (reg_info->high_bit_val) | reg_info->low_bit_val;

}

#endif
