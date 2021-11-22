/*RegistrosFactura

 Control de ingresos de una empresa, en la cual se tendra como maximo 500 facturas:
 a) El menú principal tendra las siguientes opciones:
	  1. Ingresar una factura.
	  2. Mostrar las facturas del mes.
	  3. Imprimir todas las facturas.
	  4. Salir.
	  

 b) En la opcion de 'Ingresar' solo permitira en ingreso de una sola factura;
 guardara los siguientes datos:
	numero de la factura,
	fecha de la factura,
	nombre del cliente,
	domicilio del cliente,
	RFC del cliente,
	subtotal;
	el  IVA y total seran datos calculados.

 c) En la opcion Mostrar las facturas del mes,
  mostrara solo las facturas que correspondan al mismo mes que el usuario elija,
  en caso de no encontrar facturas en ese mes debera mostrar una leyenda que
  indique que no hay facturas;  después de mostrar cada una de las facturas
  encontradas, mostrara el monto total de los campos subtotal, IVA y total
  correspondiente a las facturas de dicho mes.

 d) Evitar que el usuario introduzca el dia y mes invalidos.

 Autor: Rosales Flores Dulce Veronica
 FechaInicio: 211121			FechaFin:221121

D.E. Número de la factura de tipo entero, fecha, nombre de cliente, domicilio, RFC del cliente de tipo cadena de caracter, subtotal de tipo real.
D.S. IVA de cada factura, total de cada factura, Suma del subtotal del mes, Suma del total del mes de tipo real.
D.P. IVA = 10% */

#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


#define PORCENTAJE_IVA 16
#define SIZE_NAME 30
#define ADDRESS 50
#define RFC 13
#define MAX_FACTURAS 500
#define OPCIONES_MENU 4 									// Cantidad de las opciones de menu requeridas
#define TAMANIO_SEG_FECHA 2
#define ENERO 1
#define DICIEMBRE 12

void LimpiaBuffer() {
	// https://www.geeksforgeeks.org/clearing-the-input-buffer-in-cc/
	char ch = 0;
	do {
		ch = getchar();
	} while (ch != '\n' && ch != EOF);
}

int presentamenu(const char* opciones[OPCIONES_MENU]) {		//Imprime las opciones del menú principal
	int entrada;
	int entradavalida;
	int x;
	for (x = 1; x <= OPCIONES_MENU; x += 1) {				//Ciclo que imprime todas las opciones
		printf("%i.-%s\n", x, opciones[x - 1]);
	}
	entrada = 0;
	entradavalida = 0;
	do {
		printf("Seleccione una opcion: ");
		scanf_s("%i", &entrada);
		LimpiaBuffer();

		// Valida la entrada de la opcion seleccionada
		if (entrada >= 1 && entrada <= OPCIONES_MENU) {
			entradavalida = entrada;
		}
		else {
			printf("Entrada invalida, vuelva a intentar\n");
		}
	} while (entradavalida == 0);
	return entradavalida;
}

typedef struct {						//Estructura para almacenar fecha de registro
	int dia;
	int mes;
	int anio;
} Fecha;

typedef struct {						//Estructura para el folio (Factura)
	int numero;
	Fecha fecha; // dd/mm/aa
	char nombreCliente[SIZE_NAME];
	char domicilio[ADDRESS];
	char RFCcliente[RFC];
	float subtotal;
	float IVA;
	float total;
} Factura;

int CapturaSegmento(int min, int max, char* nombreSegmento) {

	int num;
#if defined(PRUEBA_CON_ARCHIVO)
	// prueba
	printf(" %s: ", nombreSegmento);
	scanf_s("%i", &num);
	LimpiaBuffer();	
#else
	char c;

	char segmento[TAMANIO_SEG_FECHA + 1] = { 0 };
pideFecha:
	printf(" %s: ", nombreSegmento);

	do {
		int len = strlen(segmento);
		c = _getch();
		if (isdigit(c)) {
			printf("%c", c);
			segmento[len] = c;
		}
		else if (c == '\b' && len) {
			// Backspace, desea corregir
			printf("\b");
			segmento[len - 1] = 0;
		}
		else {
			printf("\a"); // beep
		}
	} while (strlen(segmento) < TAMANIO_SEG_FECHA);

	num = atoi(segmento);
	if (num > max || num < min) {
		printf("\nError. %s incorrecto (%i a %i)", nombreSegmento, min, max);
		segmento[0] = segmento[1] = 0;
		goto pideFecha;
	}
#endif // PRUEBA_CON_ARCHIVO

	return num;
}

int EsBisiesto(int anio) {		// Esta funcion determina si el año es bisiesto
	return (anio % 4 == 0 &&	// Cada cuatro anios
		(anio % 100 != 0));		// No es siglo
}

int DiasDeMes(int mes, int anio) {		//Determina si el dia ingresado coincide con el mes (Ej: que no sea 30 de feb)

	int numDiasPorMes[] = {
		31, // enero
		28, // feb
		31, // mzo
		30, // abr
		31, // may
		30, // jun
		31, // jul
		31, // ago
		30, // sep
		31, // oct
		30, // nov
		31  // dic
	};

	if (mes<ENERO || mes > DICIEMBRE) {
		return 0; // Mes invalido
	}

	return numDiasPorMes[mes - 1] + EsBisiesto(anio);
};

Fecha CapturaFecha() {				//Ingresa la fecha en que se emite la factura
	Fecha fch = { 0 };

	printf("Capture fecha(aa/mm/dd). ");

	fch.anio = CapturaSegmento(0, 99, "anio");
	fch.mes = CapturaSegmento(1, 12, "mes");

	fch.dia = CapturaSegmento(1, DiasDeMes(fch.mes, fch.anio), "dia");

	printf(" %i/%i/%i\n", fch.anio, fch.mes, fch.dia);
	return fch;
}

void SolicitaCampo(char* formato, char* nombre, void* valor, int bufer) {
	printf("%s: ", nombre);
	if (strcmp("%s", formato) == 0) {
		// scanf no recibe espacios por defecto:
		// https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf
		/* obtener valor limitado al tamaño */
		fgets(valor, bufer, stdin);

		/* Quitar salto de línea si existe */
		int lenCaptura = strlen(valor);
		char *str = (char*)valor;
		if ((lenCaptura > 0) && (str[lenCaptura - 1] == '\n'))
			str[lenCaptura - 1] = '\0';
		return;
	}

	// De otra manera, obtener el valor regular
	scanf_s(formato, valor, bufer);
	LimpiaBuffer();
}
Factura IngresarFactura(int numero)				//Se llenan los datos generales de la factura 
{
	Factura f;
	f.numero = numero;
	f.fecha = CapturaFecha();
	SolicitaCampo("%s", "Nombre", f.nombreCliente, SIZE_NAME);
	SolicitaCampo("%s", "Domicilio", f.domicilio, ADDRESS);
	SolicitaCampo("%s", "RFC", f.RFCcliente, RFC);
	SolicitaCampo("%f", "Subtotal", &f.subtotal, sizeof(f.subtotal));
	//campos calculados
	f.IVA = f.subtotal * PORCENTAJE_IVA/100;
	f.total = f.subtotal + f.IVA;
	return f;
}

void DesplegarFactura(Factura f) {								//Se usa para imprimer la factura de un mes determinado y de todas las facturas
	printf("\n");
	printf("Numero:    %i\n", f.numero);
	printf("Fecha:     %i/%i/%i\n", f.fecha.anio, f.fecha.mes, f.fecha.dia);
	printf("Nombre:    %s\n", f.nombreCliente);
	printf("RFC:       %s\n", f.RFCcliente);
	printf("Domicilio: %s\n", f.domicilio);
	// Formato: https://www.delftstack.com/howto/c/printf-align-columns-in-c/
	printf("Subtotal:  %10.2f\n", f.subtotal);
	printf("IVA(%i %%): %10.2f\n", PORCENTAJE_IVA, f.IVA);
	printf("Total:     %10.2f\n", f.total);
}

void ImprimirFacturas(Factura* facturas) {						//Imprime las facturas de un mes determinado
	int M = 0;
	printf("Ingrese el mes del cual requiere los datos(01-12)");
	int mes = CapturaSegmento(ENERO, DICIEMBRE, "mes");			//Selecciona el mes a mostrar

	while (M < MAX_FACTURAS && facturas[M].numero)
	{
		Factura f = facturas[M++];
		if (f.fecha.mes == mes) {
			DesplegarFactura(f);
		}
	}
}

void MTodasFacturas(Factura facturas[]) {					//Imprime todas las facturas
	int M = 0;
	while (M < MAX_FACTURAS && facturas[M].numero)
	{
		DesplegarFactura(facturas[M++]);
	}
}


int main() {
	// Menu
	char* opciones[OPCIONES_MENU] =
	{
		"Ingresar una factura.",
		"Mostrar las facturas del mes.",
		"Imprimir todas las facturas.",
		"Salir."
	};

	Factura* CantFacturas = calloc(sizeof(Factura), MAX_FACTURAS);
	int numFactura = 0;
	int seleccion;
	do {
		printf("-----------------------------------------\n");
		seleccion = presentamenu(opciones);
		printf("------- %s -------\n", opciones[seleccion - 1]);
		switch (seleccion) {
		case 1:
			++numFactura;
			if (numFactura > MAX_FACTURAS)
				printf("La cantidad maxima para ingresar facturas es de 500, ha llegado al limite");
			else {
				printf("Numero de factura %i\n", numFactura);
				CantFacturas[numFactura - 1] = IngresarFactura(numFactura);
				}
				break;

		case 2:
			ImprimirFacturas(CantFacturas);
			break;

		case 3:
			MTodasFacturas(CantFacturas);
			break;

		case 4:
			printf("Adios...");
			break;
		default:
			printf("Error, invalida\n");
		}
	} while (seleccion != 4);

	free(CantFacturas);

	return 0;
}
