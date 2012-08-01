/*
 * bigint.h
 *
 *  Created on: Apr 15, 2011
 *      Author: piersaj
 *
 * 2011-04-15
 * 		FFS no bigint in STL??
 * 		OK now get this things implemented...
 */

#ifndef BIGINT_H_
#define BIGINT_H_

class Bigint{
private:
	const int LEN = 255;
	char * digits;

public:
	Bigint();
	Bigint(long val);
	Bigint(const char *);
	Bigint(Bigint &);

	Bigint &  operator=( const Bigint&) const;
	Bigint &  operator+( const Bigint&) const;
	Bigint &  operator-( const Bigint&) const;

	Bigint &  operator++();
	Bigint &  operator++(int unused);

	Bigint &  operator--();
	Bigint &  operator--(int unused);

	bool operator==(const Bigint & ) const;
	bool operator!=(const Bigint & ) const;
	bool operator>=(const Bigint & ) const;
	bool operator>(const Bigint & ) const;
	bool operator<=(const Bigint & ) const;
	bool operator<(const Bigint & ) const;

	const char* toString();
};	// class

#endif /* BIGINT_H_ */
