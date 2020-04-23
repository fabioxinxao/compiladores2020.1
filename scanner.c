#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

//reserved words
#define Rword_main 1
#define Rword_if 2
#define Rword_else 3
#define Rword_do 4
#define Rword_while 5
#define Rword_for 6
#define Rword_char 7
#define Rword_int 8 
#define Rword_float 9
#define OP_open_parent 10
#define OP_close_parent 11
#define OP_open_key 12
#define OP_close_key 13
#define OP_sum 14
#define OP_sub 15
#define OP_multi 16
#define OP_div 17
#define OP_assignment 18 //atribuicao
#define OP_equal 19 
#define OP_comma 20 //virgula
#define OP_point_virg 21 
#define OP_highest 22
#define OP_minor 23
#define OP_greater_equal 24 
#define OP_smaller_equal 25
#define OP_diferent 26
#define TYPE_ID 30
#define TAM_LEX 250

typedef struct Token
{
	char lexem[TAM_LEX];
	int type;
}token;

//procedures
token scanner(FILE *arq);
int RWord_find(char lex[]);
int OP_Find(char lex[]);

//var global
int linha = 1;
int coluna = 1;
char lookup;
token vetor;



//main begin
int main(int argc, char *argv[]){
    token ultimotoken;
    FILE *arq;
    
	if (!(arq=fopen("teste.txt","r")))
	{
		printf("Arquivo nao encontrado.\n");
		exit(1);
	}
	while (arq!=EOF)
	{
		ultimotoken=scanner(arq);
		printf("Token Lido: %s <--> Tipo: %d \n", ultimotoken.lexem, ultimotoken.type);
	}
	fclose(arq);	
	return 0;
}


token scanner(FILE *arq) {
	int i=0;
	int estado=0;
	int aceito=0;
	vetor.lexem[0]='\0';
	vetor.type=0;
	static char lookup = ' ';
	
	
	while((lookup=getc(arq))!=EOF)	
	{
		if (lookup==' ' || lookup=='\t')//tirar espaços em branco
			continue;	
		else if(lookup=='\n')
		{//somente incrementar linha
			linha++;
			coluna=1;
		}
		
		else if(isalpha(lookup))//inicio identificador ou palavra reservada
		{
			i=0;
			coluna++;
			do
			{
				vetor.lexem[i]=lookup;
				i++;
				lookup=getc(arq);
				coluna++;
				if(i>=TAM_LEX)
				{
					printf("Erro. tamanho do buffer excedido.\n");
					fclose(arq);
					exit(0);
				}
			}while(isalpha(lookup) || isdigit(lookup));
			vetor.lexem[i]='\0';
			if(lookup!=EOF)
				ungetc(lookup,arq);
			else
				lookup=0;
			vetor.type=RWord_find(vetor.lexem);
			if(vetor.type==-1)
				vetor.type=TYPE_ID;
			return vetor;
		}//fim identificador ou palavra reservada
		
		else if(isdigit(lookup)!=0 || lookup=='.')//int ou float
		{
			while(isdigit(lookup)!=0)
			{
				vetor.lexem[i]=lookup;
				i++;
				coluna++;
				lookup=getc(arq);
				//continuacao da sequencia de digitos
			}
			if(lookup=='.')//inicio de float, nao pode ter outro . 
			{
				vetor.type=Rword_float;
				vetor.lexem[i]=lookup;
				i++;
				coluna++;
				lookup=getc(arq);
				if(isdigit(lookup)!=0)
				{
					printf("Float mal formado, deveria vir um numero depois do . linha:%d coluna:%d",linha,coluna);
					fclose(arq);
					exit(0);
				}
				do
				{
					vetor.lexem[i]=lookup;
					i++;
					coluna++;
					lookup=getc(arq);
					if(lookup=='.')
					{
						printf("Problema com um float, linha:%d coluna %d",linha,coluna);
						fclose(arq);
						exit(0);
					}
				}while(isdigit(lookup)==0);
				fseek(arq,-1,SEEK_CUR);
				return vetor;	
			}
			else
			{
				vetor.type= Rword_int;
				coluna++;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
		}//fim else if int float
		
		else if (lookup=='<')//eh um operador relacional, verificar qual
		{
			vetor.lexem[i]=lookup;
			i++;
			lookup=getc(arq);
			coluna++;
			if (lookup=='='){
				vetor.lexem[i]=lookup;
				i++;
				vetor.lexem[i]='\0';
				vetor.type=OP_smaller_equal;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
			else{
				vetor.lexem[i++]='\0';
				vetor.type=OP_minor;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
		}
		else if (lookup=='>')//eh um operador relacional, verificar qual
		{
			vetor.lexem[i]=lookup;
			i++;
			lookup=getc(arq);
			coluna++;
			if (lookup=='=')
			{
				vetor.lexem[i]=lookup;
				i++;
				vetor.lexem[i]='\0';
				vetor.type=OP_greater_equal;
				return vetor;
			}
			else
			{
				vetor.lexem[i++]='\0';
				vetor.type=OP_highest;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
		}
		else if (lookup=='!')//deve ser operador "diferente"
		{
			vetor.lexem[i]=lookup;
			i++;
			lookup=getc(arq);
			coluna++;
			if (lookup=='=')
			{
				vetor.lexem[i]=lookup;
				i++;
				vetor.lexem[i]='\0';
				vetor.type=OP_diferent;
				return vetor;
			}//fim de operador "diferente"
			else //nao veio o sinal de = , entao eh um token invalido
			{
				printf("era esperado o = mas apareceu: '%c' error123,\n",lookup);
				fclose(arq);
				exit(0);
			}
		}//
		else if(lookup == '=')//pode ser operador comparacao ou atribuicao
		{
			vetor.lexem[i]=lookup;
			i++;
			coluna++;
			lookup=getc(arq);
			if (lookup=='=')
			{//eh operador de comparacao
				vetor.lexem[i]=lookup;
				coluna++;
				i++;
				vetor.lexem[i]='\0';
				vetor.type=OP_equal;
				return vetor;
			}
			else
			{//eh operador de atribuicao
				vetor.lexem[i]='\0';
				vetor.type=OP_assignment;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
		}//fim de IF comparacao ou atribuicao
		else if (lookup=='+')//operador de soma
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_sum;
			return vetor;
		}//fim de IF operacao soma
		else if (lookup=='-')//operador de subtracao
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_sub;
			return vetor;
		}//fim de IF operador subtracao
		else if (lookup=='*')//operador de multiplicacao
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_sub;
			return vetor;
		}//fim de IF operador multiplicacao
		else if (lookup=='(')//operador abre parentesis
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_open_parent;
			return vetor;
		}//fim de IF operador abre parentesis
		else if (lookup==')')//operador fecha parantesis
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_close_parent;
			return vetor;
		}//fim de IF operador fecha parentesis
		else if (lookup=='{')//operador abre chave
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_open_key;
			return vetor;
		}//fim de IF operador abre chave
		else if (lookup=='}')//operador fecha chave
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_close_key;
			return vetor;
		}//fim de IF operador fecha chave
		else if (lookup==',')//operador virgula
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_comma;
			return vetor;
		}//fim de IF operador virgula
		else if (lookup==';')//operador ponto virgula
		{
			vetor.lexem[i]=lookup;
			i++;
			vetor.lexem[i]='\0';
			vetor.type=OP_point_virg;
			return vetor;
		}////fim if de operador ponto virgula
		else if (lookup==39)//ver se eh uma palavra reservada char
		{
			vetor.lexem[i]=lookup;
			lookup=getc(arq);
			if (isalpha(lookup)){
				vetor.lexem[i]=lookup;
				i++;
				lookup=getc(arq);
			}
			if (lookup==39){
				vetor.lexem[i]=lookup;
				i++;
				vetor.lexem[i]='\0';
				vetor.type=Rword_char;
				return vetor;
			}
			else{
				printf("era esperado o ' mas apareceu: '%c' error123,\n",lookup);
				fclose(arq);
				exit(1);
			}
		}//fim if de palavra reservada char
		//comecar tratamento de /
		else if (lookup=='/')//pode ser comentario ou divisao
		{	
			lookup=getc(arq);
			if(lookup=='/')
			{//eh um comentario de linha
				do
				{
					lookup=getc(arq);
					coluna++;
					if(lookup=='\n')//descarta tudo ate achar o \n
					{
						linha++;
						break;
					}
					else if(lookup==EOF)//se chegou ao fim do arquivo, pode retornar sem erro
					{
						printf("chegou ao fim de arquivo com um comentario");
						exit(1);
					}
				}while(lookup!=EOF);
			}
			else if(lookup=='*')//eh um comentario multi linha
			{
				do
				{
					if(lookup=='*')
					{
						lookup=getc(arq);
						if(lookup='/')
						{
							linha++;
							break;
						}
						else if(lookup==EOF)
						{
							printf("Chegou ao fim de arquivo em um comentario");
							fclose(arq);
							exit(1);
						}
					}
					else if(lookup==EOF)
					{
							printf("Chegou ao fim de arquivo em um comentario");
							fclose(arq);
							exit(0);
					}
					else
					{
						lookup=getc(arq);
						coluna++;	
					}	
				}while(lookup!=EOF);
			}
			else //eh um operador de divisao
			{
				vetor.lexem[i]=lookup;
				vetor.type=OP_div;
				linha++;
				fseek(arq,-1,SEEK_CUR);
				return vetor;
			}
		}
		else
		{
			printf("character %c invalido, localizado na linha: %d e coluna: %d", lookup,linha,coluna);
			fclose(arq);
			exit(0);
		}
			
			
			
			

			
	}// fim while principal

}//fim do procedimendo scanner
	
	
int RWord_find(char lex[])
{
	if (strcmp(lex, "main") == 0)
		return Rword_main;
	if (strcmp(lex, "else") == 0)
		return Rword_else;
	if (strcmp(lex, "if") == 0)
		return Rword_if;
	if (strcmp(lex, "do") == 0)
		return Rword_do;
	if (strcmp(lex, "while") == 0)
		return Rword_while;
	if (strcmp(lex, "int") == 0)
		return Rword_int;	
	if (strcmp(lex, "float") == 0)
		return Rword_float;
	if (strcmp(lex, "char") == 0)
		return Rword_char;
	if (strcmp(lex, "for") == 0)
		return Rword_for;
	return -1;

}
