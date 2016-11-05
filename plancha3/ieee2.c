union ieee754_float {
	float f;
	struct {
#if		__BYTE_ORDER == __LITTLE_ENDIAN
		unsigned int mantissa:23;
		unsigned int exponent:8;
		unsigned int negative:1;
#endif
	} ieee;
};

