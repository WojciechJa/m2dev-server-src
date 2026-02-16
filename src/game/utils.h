#pragma once
#include <math.h>
#include <string>

#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) = (var) ^ (bit))

inline float DISTANCE_SQRT(long dx, long dy)
{
    return ::sqrt((float)dx * dx + (float)dy * dy);
}

inline int DISTANCE_APPROX(int dx, int dy)
{
	int min, max;

	if (dx < 0)
		dx = -dx;

	if (dy < 0)
		dy = -dy;

	if (dx < dy)
	{
		min = dx;
		max = dy;
	}
	else
	{
		min = dy;
		max = dx;
	}

    // coefficients equivalent to ( 123/128 * max ) and ( 51/128 * min )
    return ((( max << 8 ) + ( max << 3 ) - ( max << 4 ) - ( max << 1 ) +
		( min << 7 ) - ( min << 5 ) + ( min << 3 ) - ( min << 1 )) >> 8 );
}

#ifndef OS_WINDOWS
inline WORD MAKEWORD(BYTE a, BYTE b)
{
	return static_cast<WORD>(a) | (static_cast<WORD>(b) << 8);
}
#endif

extern void set_global_time(time_t t);
extern time_t get_global_time();

extern int	dice(int number, int size);
extern size_t str_lower(const char * src, char * dest, size_t dest_size);

extern void	skip_spaces(char **string);

extern const char *	one_argument(const char *argument, char *first_arg, size_t first_size);
extern const char *	two_arguments(const char *argument, char *first_arg, size_t first_size, char *second_arg, size_t second_size);
extern const char * three_arguments(const char* argument, char* first_arg, size_t first_size, char* second_arg, size_t second_size, char* third_flag, size_t third_size);
extern const char *	first_cmd(const char *argument, char *first_arg, size_t first_arg_size, size_t *first_arg_len_result);

#ifdef ADMINPANEL_MOD_CREATE_ITEM_ASLAN
extern const char* adminpanel_create_item_arguments(const char* argument, char* vnum_arg, size_t vnum_size,
	char* count_arg, size_t count_size,
	char* socket0_arg, size_t socket0_size,
	char* socket1_arg, size_t socket1_size,
	char* socket2_arg, size_t socket2_size,
	char* applytype0_arg, size_t applytype0_size,
	char* applyvalue0_arg, size_t applyvalue0_size,
	char* applytype1_arg, size_t applytype1_size,
	char* applyvalue1_arg, size_t applyvalue1_size,
	char* applytype2_arg, size_t applytype2_size,
	char* applyvalue2_arg, size_t applyvalue2_size,
	char* applytype3_arg, size_t applytype3_size,
	char* applyvalue3_arg, size_t applyvalue3_size,
	char* applytype4_arg, size_t applytype4_size,
	char* applyvalue4_arg, size_t applyvalue4_size,
	char* applytype5_arg, size_t applytype5_size,
	char* applyvalue5_arg, size_t applyvalue5_size,
	char* applytype6_arg, size_t applytype6_size,
	char* applyvalue6_arg, size_t applyvalue6_size,
	char* playername_arg, size_t playername_size
);
#endif

extern int CalculateDuration(int iSpd, int iDur);

extern int parse_time_str(const char* str);

extern bool WildCaseCmp(const char *w, const char *s);

namespace utils
{
	inline bool iequals(const std::string& a, const std::string& b) {
		return a.size() == b.size()
			&& std::equal(a.begin(), a.end(), b.begin(),
				[](unsigned char ac, unsigned char bc) {
			return std::tolower(ac) == std::tolower(bc);
		});
	}
}

