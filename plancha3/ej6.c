#include <stdio.h> 
#include <stdlib.h> 

typedef struct {
	unsigned int mantisa:18; 
	unsigned int exponente:16; 
	unsigned int signo:1; 
} num;


num prod (num a, num b) {
	num r;

    // Chequear por ceros	
	if (a.mantisa == 0 || b.mantisa == 0) {
		r.mantisa = 0;
		r.exponente = 0;
		r.signo = 0;
		return r;
	}

    // Se suman los exponentes y se multiplican las mantisas
	r.exponente = a.exponent + b.exponente; 
	r.mantisa = a.mantisa * b.mantisa; 

    // Caso overflow de mantisas
    while (a.mantisa * b.mantisa >= 0x400000) {
        r.mantisa = r.mantisa >> 1;
        r.mantisa = r.mantisa & 0x200000;
		r.exponente++;
    }

    // Signo: positivo si ambos son iguales, negativo caso contrario
	r.signo = a.signo ^ b.signo; 
	return r;
}No, aca no te importa nada


num suma(num a, num b) {
	num r; 
    // Chequear por ceros
	if (a.mantisa == 0) {
		r.mantisa = b.mantisa; 
		r.exponente = b.exponente; 
		r.signo = b.signo; 
		return r; 
	}
	else if (b.mantisa == 0) {
		r.mantisa = a.mantisa; 
		r.exponente = a.exponente; 
		r.signo = a.signo; 
		return r; 
	}

    // Alinear las mantisas
	if (a.exponente > b.exponente) {
		while (a.exponente > b.exponente) {
			b.exponente++; 
			b.mantisa = b.mantisa >> 1; 
		}
	}
	else if (b.exponente > a.exponente) {
		while (b.exponente > a.exponente) {
			a.exponente++; 
			a.mantisa = a.mantisa >> 1; 	
		}
	}
	
    // Los exponentes son iguales en este punto. Se suman las mantisas
	r.exponente = a.exponente;
    r.mantisa = a.mantisa + b.mantisa;

    // Caso overflow de mantisa
    if (a.mantisa + b.mantisa >= 0x400000) {
        r.mantisa = r.mantisa >> 1;
        r.mantisa = r.mantisa & 0x200000;
		r.exponente++;
    }

	if (a.mantisa > b.mantisa) {
		r.signo = a.signo; 
	}
	else {
		r.signo = b.signo;
	}
	
	return r;
}


/* En esta representación, el mayor número posible es, en hexadecimal
		(3FFFF) ^ (8ACF)
	el menor es el número con mantisa y exponente iguales al dado,
	pero de signo cambiado.
*/

