#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <iostream>

static gmp_randstate_t randstate;
typedef unsigned long long ull;

//Schlüssel Klasse
struct key {
  mpz_t exponent;
  mpz_t mod;
};


void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}


//Funktion zum generieren von Schlüsseln
void genKey(unsigned int bitSize, struct key *pub, struct key *priv) {
  //p, q werden erstellt und initialisiert
  mpz_t p, q;
  mpz_init(p);
  mpz_init(q);

  std::cout << bitSize << std::endl;
  unsigned int bitNew = bitSize >> 1;
  std::cout << bitNew << std::endl;

  //es werden zufällige zahlen mit dem gegebenen bitwert p und q zugewiesen
  //bitsize / 2
  //Beispiel 1024 -> 10000000000
  mpz_urandomb(p, randstate, bitSize >> 1);
  mpz_urandomb(q, randstate, bitSize >> 1);

  //versteh ich nicht ganz, bugfix von stackoverflow ; )
  mpz_setbit(p, (bitSize >> 1) - 1);
  mpz_setbit(p, (bitSize >> 1) - 2);
  mpz_setbit(p, (bitSize >> 1) - 1);
  mpz_setbit(p, (bitSize >> 1) - 2);

  //p und q werden die nächsten primzahlen zugewiesen, hinter der zufälligen zahl aus zeile 23f
  mpz_nextprime(p, p);
  mpz_nextprime(q, q);

  //n wird erstellt und initialisiert und es werden p und q multipliziert
  mpz_t n; mpz_init(n);
  mpz_mul(n, p, q);

  //der exponent des öff schl. ist 65537, also die höchste zahl eines uint, wird beim richtigen rsa verfahren so gemacht, (habe ich online als tipp erhalten)
  mpz_set_ui(pub -> exponent, 65537ul);
  //der modulo wert des öff schl. ist n
  mpz_set(pub -> mod, n);
  //der modulo wert des priv. schl. ist auch n
  mpz_set(priv -> mod, n);

  //es wird 1 von p und q abgezogen
  mpz_sub_ui(p, p, 1ul);
  mpz_sub_ui(q, q, 1ul);

  //der größte gemeinsame teiler gefunden von p und q
  mpz_lcm(p, p, q);
  mpz_invert(priv -> exponent, pub -> exponent, p);

  mpz_clear(p); mpz_clear(q);
}


void encrypt(mpz_t C, const mpz_t M, const struct key k) {
  mpz_powm(C, M, k.exponent, k.mod);
}

void decrypt(mpz_t M, const mpz_t C, const struct key k) {
  mpz_powm(M, C, k.exponent, k.mod);
}

int main(int argc, char **argv){
  //die random methoden werden initialisiert und geseeded mit der aktuellen cpu time
  gmp_randinit_mt(randstate);
  gmp_randseed_ui(randstate, time(NULL));

  //der private und öff schl. werden erstellt
  struct key pub, priv;

  //alle werte der schl. werden initialisiert
  mpz_init(pub.mod); mpz_init(pub.exponent);
  mpz_init(priv.mod); mpz_init(priv.exponent);

  int bitAnzahl = 0;

  printf("------------\n");
  printf("RSA Public und Private Schlüssel generator von Ben \n \n");
  printf("Mit welcher Bit-Anzahl soll ein Schlüssel erstellt werden? \n");
  std::cin >> bitAnzahl;

  //schl. wird generiert, es werden privater und öff schl. mitgegeben
  genKey(bitAnzahl, &pub, &priv);
  printf("------------\n");
  gmp_printf("Privat: \n d: %Zd \n n: %Zd \n", priv.exponent, priv.mod);
  printf("------------\n");
  gmp_printf("Öffentlich: \n e: %Zd \n n: %Zd \n", pub.exponent, pub.mod);
  printf("------------\n");

  while(true) {
    printf("\n1. Verschlüsseln \n");
    printf("2. Entschlüsseln \n");
    printf("3. Stop \n");
    int choice = 0;
    std::cin >> choice;
    if(choice == 1) {
      printf("Bitte Text eingeben: \n");
      std::string text;
      std::cin >> text;
      //jeder char wird einzeln verschl.
      for(char ch : text) {
        //M = Klartext und C = Cypher
        mpz_t M; mpz_init_set_ui(M, int(ch));
        mpz_t C; mpz_init(C);
        //es wird encry. und dann geprinted
        encrypt(C, M, pub);
        gmp_printf("\nOutput: %Zd \n", C);
        printf("ASCII: %d \n", int(ch));
      }
      continue;
    }
    else if (choice == 2) {
      printf("Bitte Verschlüsselten Text eingeben: \n");
      printf("Nicht den gesamten Text eingeben, nur ein Block nach dem anderen !!!!!!!!!!!! \n");
      std::string input;
      std::cin >> input;
      //M = Klartext und C = Cypher
      mpz_t C; mpz_init_set_str(C, input.c_str(), 10);
      mpz_t M; mpz_init(M);
      decrypt(M, C, priv);
      //mpz_t wird zu einem char umgewandelt
      long unsigned int out = mpz_get_ui(M);
      char c = out;
      printf("Output: %c\n", c);
      //gmp_printf("Output: %Zd \n", M);
      continue;
    }
    else {break;}
  }
  return 0;
}
