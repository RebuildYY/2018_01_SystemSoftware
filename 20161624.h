#include<stdio.h>
#include<stdlib.h>
#include<String.h>
#include<sys/stat.h>
//#include<dirent.h>
/*****************************************/
#define MAX_STRING_SIZE 30
#define MAX_MEM_ROW 65536
#define MAX_MEM_COL 16
#define MAX_MEM_SIZE 1048575
#define MAX_HASH_ROW 20
/*****************************************/
typedef struct Historytable {
	char str[MAX_STRING_SIZE];
	struct Historytable* link;
}historytable;
historytable* his_head;
typedef struct Opcodetable {
	char num[3];
	char name[6];
	char fm[4];
	struct Opcodetable* link;
}opcodetable;
opcodetable* op_table;
typedef struct Symboltable {
	char label[10];
	int loc;
	struct Symboltable* link;
}symboltable;
symboltable* sym_table;
/*****************************************/
int dump_addr = 0;
int locctr = 0;
int loc_table[1000];
int obj_size[1000];
int base = 0;
char **virtual_mem;
/*****************************************/
int help(void);
//help와 관련된 일들을 수행하며, 사용가능한 shell 명령어를 출력한다.
int directory(void);
//dir와 관련되 일들을 수행하며, 현재 디렉토리에 있는 파일들을 출력한다.
int history(char* str);
//history와 관련되 일들을 수행하며, 지금까지 사용된 올바른 형식의 명령어와 인자들을 저장한다.
//linked list를 사용하며,앞에 num을 붙여 출력한다.
int dump(char* start, char* end);
//dump와 관련된 일들을 수행하며, 할당된 메모리의 내용을 출력한다.
//인자로 start, end를 받을 수 있다.
//인자가 없는 경우, 기본적으로 10라인(160개)이 출력한다. 이후에 dump가 쓰일경우 이어서 그 뒤부분을 출력한다.
//start만 있는 경우, start로 부터 10라인을 출력한다.
//start, end가 쓰인 경우, start에서 end까지 출력한다.
//start > end인 경우, 에러처리한다.
//주소를 넘어간 경우 주소의 끝(0xFFFFF)까지만 출력한다.
//dump의 실행으로 출력된 마지막 address는 내부에 저장하고 있다.
//dump 출력시 binary check를 하여 exception error처리한다.
int edit(char* address, char* value);
//address, value를 인자로 받으며, 해당 address의 값을 value값으로 변경한다.
//address, value는 16진수의 값이다.
int fill(char* start, char* end, char* value);
//start, end, value를 인자로 받으며, start에서 end까지 value의 값으로 채운다.
//start, end, value는 16진수의 값이다.
int reset(void);
//memory의 값을 모두 00으로 한다. 00은 16진수이다.
int opcode(char* mnomonic);
//mnomonic을 인자로 받으며, 이는 char*인다.
//해당하는 명령어의 opcode를 출력한다.(16진수)
int opcodelist(void);
//Hash Table을 기반으로 하여, hashtable에 저장된 opcode를 출력한다.
int assemble();
//filename을 인자로 받으며, filename에 해당하는 소스 파일을 읽어서 object파일과 리스팅 파일을 만든다.
//소스 파일의 확장자는 .asm이다.
//리스팅 파일의 파일명은 소스파일과 동일하고 확장자는 .lst이다.
//object 파일의 파일명은 소스파일과 동일하고 확장자는 .obj이다.
//소스파일에 에러가 존재하는 경우, 리스팅 파일과 object파일을 생성하지 않고, 에러 내용을 화면에 출력한다.
//에러 발생시 바로 명령이 종료된다.
//에러의 내용은 디버깅을 위해서 어떤 라인에서 에러가 발생했는지를 출력한다.
int type(char* filename);
//filename을 인자로 받으며, 파일을 현재 디렉터리에서 읽어서 화면에 출력한다.
//현재 디렉토리에 존재하지 않을 경우 에러메세지를 출력한다.
int symbol(void);
//assemble과정에서 생성된 symbol table을 화면에 출력한다.
//symbol의 출력은 symbol을 기준으로 내림차순으로 정렬이 되어야 한다.
//형식 : (\t)RETADR(\t)0030
int progaddr();
//address를 인자로 받으며, loader 또는 rum 명령어를 수행할 때 시작하는 주소를 저장합니다.
//sicsim이 시작되면 default로 progaddr는 0x00 주소로 지정된다.
int loader();
//object filename들이 최대 3개까지 인자로 받을 수 있다.
//object파일을 읽어서 linking작업을 수행 후, 가상 메모리(1M)에 그 결과를 기록한다.(교재의 pass1, pass2)
//loader가 성공적이면, load map을 화면에 출력한다.
//에로가 존재할 경우, 에러 내용이 화면에 출력된다.
int run();
//loader 명령어의 수행으로 메모리에 load된 프로그램을 실행한다.
//progaddr 명령어로 지정한 주소부터 실행된다.
//실행 결과로써 register 상태를 화면에 출력한다.(A, X, L, PC, B, S, T)
//break point까지 실행되고 Breakpoint가 없으면 프로그램 끝까지 실행된다.
int bp();
//break point와 관련되 일을 처리한다.
//bp만 쓰였을 경우 모든 break point를 출력한다.
//+[address]이면 해당 주소에 break point를 생성한다.
//+"clear"이면 모든 break point를 삭제한다.
int split_inst(char* str, char** factor);
// str(입력받은 문장)을 나누어 factor(인자)들로 나누어 반환한다.
//factor은 배열로 최대 3개까지 저장할 수 있게 만든다.
//해당하는 명령어에 대응하는 번호를 return한다.
int get_opcode(char* mnemonic);
int get_sym_loc(char* label);
int pass1(FILE* fp);
int pass2(FILE* fp, FILE* fp2, FILE* fp3);
int load_pass1();
int load_pass2();
int trans_str_to_int(char* str);
int trans_str_to_dec(char* str);
int start, end;
//16진수 문자열을 10진수의 정수형으로 바꾸어준다.
int hash_func(char* str) {
	int i, value = 0;
	for (i = 0; i<strlen(str); i++) {
		value += (int)str[i];
	}
	return (value % 20);
}
int make_opcode_table(void) {
	FILE* fp;
	int num = 0, key;
	char name[6];
	char fm[4];
	opcodetable* temp;
	opcodetable* target;
//	fp = fopen("pcode.txt", "r");
	if (!(fp = fopen("opcode.txt", "r"))) {
		fputs("opcde.txt파일 오픈 실패\n", stderr);
		return -1;
	}
	while (!feof(fp)) {
		temp = (opcodetable*)malloc(sizeof(opcodetable));
		temp->link = NULL;
		fscanf(fp, "%s %s %s\n", temp->num, temp->name, temp->fm);
		key = hash_func(temp->name);
		target = op_table[key].link;
		if (target == NULL) {
			op_table[key].link = temp;
		}
		else {
			while (target->link != NULL) {
				target = target->link;
			}
			target->link = temp;
		}
	}
	fclose(fp);
	return 0;
}
int get_opcode(char* mnemonic) {
	int key;
	opcodetable* target;
	key = hash_func(mnemonic);
	target = op_table[key].link;
	while (1) {
		if (strcmp(target->name, mnemonic) == 0) {
			return trans_str_to_int(target->num);
		}
		if (target->link == NULL) { break; }
		target = target->link;
	}
	return -1;
}
int get_sym_loc(char* label) {
	symboltable* target;
	int key;
	key = hash_func(label);
	target = sym_table[key].link;
	while (target != NULL) {
		if (strcmp(target->label, label) == 0) {
			return target->loc;
		}
		target = target->link;
	}
	return -1;
}
int get_reg(char* reg) {
	if (strcmp(reg, " ") == 0) { return 0; }
	else if (strcmp(reg, "A") == 0) { return 0; }
	else if (strcmp(reg, "X") == 0) { return 1; }
	else if (strcmp(reg, "L") == 0) { return 2; }
	else if (strcmp(reg, "B") == 0) { return 3; }
	else if (strcmp(reg, "S") == 0) { return 4; }
	else if (strcmp(reg, "T") == 0) { return 5; }
	else if (strcmp(reg, "F") == 0) { return 6; }
	else if (strcmp(reg, "PC") == 0) { return 8; }
	else if (strcmp(reg, "SW") == 0) { return 9; }
	else { return -1; }
}
char trans_int_to_hex(int a) {
	if (a >= 10) {
		return (char)(a + 55);
	}
	else {
		return (char)(a + 48);
	}
}
int make_objcode(char* objcode, int op, int mid, int lvalue, int fm) {
	if (fm == 3) {
		objcode[0] = trans_int_to_hex(op / 16);
		objcode[1] = trans_int_to_hex(op % 16);
		objcode[2] = trans_int_to_hex(mid);

	}
	else if (fm == 4) {

	}
}
int pass1(FILE* fp) {
	char str[100];
	char **factor;
	char opcode[31];
	char label[31];
	char operand[31];
	char* temp;
	int i, fm = 0, key, find = 0,plus = 0, loc_i = 0, size = 0, size_i = 0;
	opcodetable* optarget;
	opcodetable* optemp;
	symboltable* starget;
	symboltable* stemp;
	factor = (char**)malloc(sizeof(char*) * 3);
	for (i = 0; i < 3; i++) {
		factor[i] = (char*)malloc(sizeof(char) * MAX_STRING_SIZE);
	}
	while (!feof(fp)) {
		plus = find = 0;
		fgets(str, sizeof(str), fp);
		if (strcmp(str, "\n") != 0 && strcmp(str, "\0") != 0) {
			strcpy(opcode, "\0");
			strcpy(label, "\0");
			strcpy(operand, "\0");
			if (str[0] != '.') {
				factor[0] = strtok(str, " ");
				key = hash_func(factor[0]);
				optarget = op_table[key].link;
				while (optarget != NULL) {
					if (strcmp(optarget->name, factor[0]) == 0) {
						find = 1; break;
					}
					optarget = optarget->link;
				}
				if (find == 0) {
					factor[1] = strtok(NULL, " ");
					factor[2] = strtok(NULL, " ");
				}
				else {
					factor[1] = strtok(NULL, "\n");
					factor[2] = NULL;
				}
				factor[0] = strtok(factor[0], "\n");
				factor[1] = strtok(factor[1], "\n");
				factor[2] = strtok(factor[2], "\n");
				if (factor[2] != NULL) {
					strcpy(label, factor[0]);
					if (factor[1][0] == '+') {
						plus = 1;
					}

					temp = strtok(factor[1], "+");
					strcpy(opcode, temp);
					strcpy(operand, factor[2]);
				}
				else {
					strcpy(label, "\0");
					if (factor[0][0] == '+') {
						plus = 1;
					}
					
					temp = strtok(factor[0], "+");
					strcpy(opcode, temp);
					if (factor[1] == NULL) {
						strcpy(operand, "\0");
					}
					else {
						strcpy(operand, factor[1]);
					}
				}
				//이제 opcode에 따라 locctr값을 계산
				if (strcmp(opcode, "START") == 0) {
					locctr = trans_str_to_int(operand);
					loc_table[loc_i] = locctr;
					start = locctr;
					loc_i++;
				}
				else {
					if (strcmp(opcode, "END") == 0) { end = loc_table[loc_i-1]; break; }
					if (strcmp(label, "\0") != 0) {
						key = hash_func(label);
						starget = sym_table[key].link;
						if (starget != NULL) {
							while (1) {
								if (strcmp(starget->label, label) == 0) {
									fprintf(stderr, "겹치는 label: %s\n", label);
									return -1;
								}
								if (starget->link == NULL) {
									break;
								}
								starget = starget->link;	
							}
							stemp = (symboltable*)malloc(sizeof(symboltable));
							strcpy(stemp->label, label);
							stemp->loc = locctr;
							stemp->link = NULL;
							starget->link = stemp;
						}
						else {
							stemp = (symboltable*)malloc(sizeof(symboltable));
							strcpy(stemp->label, label);
							stemp->loc = locctr;
							stemp->link = NULL;
							sym_table[key].link = stemp;
						}
					}
					
					key = hash_func(opcode);
					optarget = op_table[key].link;
					while (optarget != NULL) {
						if (strcmp(optarget->name, opcode) == 0) {
							if (strcmp(optarget->fm, "3/4") == 0) {
								if (plus == 1) { fm = 4;  }
								else { fm = 3; }
							}
							else if (strcmp(optarget->fm, "2") == 0) { fm = 2; }
							else if (strcmp(optarget->fm, "1") == 0) { fm = 1; }
							if (size + fm > 30) { obj_size[size_i] = size; size = fm; size_i++; } else { size += fm; }
							locctr += fm;
						}
						else {	
							if (strcmp(opcode, "BASE") == 0) { break; }
							else if (strcmp(opcode, "WORD") == 0) {
								if (size + 3 > 30) { obj_size[size_i] = size; size = 3; size_i++; }
								else { size += 3; }
								locctr += 3; break; 
							}
							else if (strcmp(opcode, "RESW") == 0) { obj_size[size_i] = size; size = 0; size_i++; locctr += 3 * trans_str_to_dec(operand); break; }
							else if (strcmp(opcode, "RESB") == 0) { obj_size[size_i] = size; size = 0; size_i++; locctr += trans_str_to_dec(operand); break; }
							else if (strcmp(opcode, "BYTE") == 0) {
								temp = strtok(operand, "\'");
								if (strcmp(temp, "X") == 0) {
									temp = strtok(NULL, "'");
									locctr += strlen(temp) / 2;
									if (size + strlen(temp)/2 > 30) { obj_size[size_i] = size; size = strlen(temp)/2; size_i++; }
									else { size += strlen(temp)/2; }
								}
								else if (strcmp(temp, "C") == 0) {
									temp = strtok(NULL, "'");
									locctr += strlen(temp);
									if (size + strlen(temp) > 30) { obj_size[size_i] = size; size = strlen(temp); size_i++; }
									else { size += strlen(temp); }
								}
								break;
							}
						}
						optarget = optarget->link;
					}
				}
			}//"."이 아니면 end
		}//"\n", "\0" end
		loc_table[loc_i] = locctr;
		loc_i++;
	}
	return 0;
}
int pass2(FILE* fp, FILE* fp2, FILE* fp3) {
	char str[100];
	char temp_str[100];
	char **factor;
	char opcode[31];
	char label[31];
	char operand[31];
	char objcode[9];
	char reg1[3], reg2[3];
	char* temp;
	int i, fm = 0, key, find = 0, loc_i = 0, row = 5, type = 0, plus = 0, op = 0, base = 0, mid = 0, lvalue = 0, size = 0, size_i = 0;
	opcodetable* optarget;
	opcodetable* optemp;
	symboltable* starget;
	symboltable* stemp;
	factor = (char**)malloc(sizeof(char*) * 3);
	for (i = 0; i < 3; i++) {
		factor[i] = (char*)malloc(sizeof(char) * MAX_STRING_SIZE);
	}
	while (!feof(fp)) {
		plus = type = find = op = mid = lvalue = 0;
		reg1[0] = ' '; reg2[0] = ' ';
		fgets(str, sizeof(str), fp);
		
		strcpy(temp_str, str);
		temp = strtok(temp_str, "\n");
		strcpy(temp_str, temp);
		if (strcmp(str, "\n") != 0 && strcmp(str, "\0") != 0) {
			strcpy(opcode, "\0");
			strcpy(label, "\0");
			strcpy(operand, "\0");
			if (str[0] != '.') {
				factor[0] = strtok(str, " ");
				key = hash_func(factor[0]);
				optarget = op_table[key].link;
				while (optarget != NULL) {
					if (strcmp(optarget->name, factor[0]) == 0) {
						find = 1; break;
					}
					optarget = optarget->link;
				}
				if (find == 0) {
					factor[1] = strtok(NULL, " ");
					factor[2] = strtok(NULL, " ");
				}
				else {
					factor[1] = strtok(NULL, "\n");
					factor[2] = NULL;
				}
				factor[0] = strtok(factor[0], "\n");
				factor[1] = strtok(factor[1], "\n");
				factor[2] = strtok(factor[2], "\n");
				if (factor[2] != NULL) {
					strcpy(label, factor[0]);
					if (factor[1][0] == '+') {
						plus = 1;
						temp = strtok(factor[1], "+");
					}
					else {
						temp = factor[1];
					}
					strcpy(opcode, temp);
					for (i = 0; i < strlen(factor[2]); i++) {
						if (factor[1][i] != ' ' && factor[1][i] != '\t'&& factor[1][i] != 32) {
							strcpy(factor[2], (factor[2]+i));
							break;
						}
					}
					if (factor[2][0] == '@') {
						type = 1;
						temp = strtok(factor[2], "@");
					}
					else if (factor[2][0] == '#') {
						type = 2;
						temp = strtok(factor[2], "#");
					}
					else {
						type = 0;
						temp = factor[2];
					}

					strcpy(operand, temp);
				}
				else {//인자가 2개
					strcpy(label, "\0");
					if (factor[0][0] == '+') {
						plus = 1;
						temp = strtok(factor[0], "+");
					}
					else {
						temp = factor[0];
					}
					strcpy(opcode, temp);
					if (factor[1] != NULL) {
						for (i = 0; i < strlen(factor[1]); i++) {
							if (factor[1][i] != ' ' && factor[1][i] != '\t'&& factor[1][i] != 32) {
								strcpy(factor[1], (factor[1] + i));
								break;
							}
						}
					}

					if (factor[1] == NULL) {
						strcpy(operand, "\0");
					}
					else {
						if (factor[1][0] == '@') {
							type = 1;
							temp = strtok(factor[1], "@");
						}
						else if (factor[1][0] == '#') {
							type = 2;
							temp = strtok(factor[1], "#");
						}
						else {
							type = 0;
							temp = factor[1];
						}
						strcpy(operand, temp);
					}
				}	
				key = hash_func(opcode);
				optarget = op_table[key].link;
				while (optarget != NULL) {
					if (strcmp(optarget->name, opcode) == 0) {
						if (strcmp(optarget->fm,"1") == 0) { fm = 1; }
						else if (strcmp(optarget->fm, "2") == 0) { fm = 2; }
						else {
							if (plus == 1) { fm = 4; }
							else { fm = 3; }
						}
						break;
					}
					optarget = optarget->link;
				}
			
				//이제 opcode에 따라 locctr값을 계산
				if (strcmp(opcode, "START") == 0) {
					fprintf(fp2, "%d\t%04X\t%s\n", row, loc_table[loc_i], temp_str);
					fprintf(fp3, "H%-6s%06X%06X\n", label, start, end);
					printf("H%-6s %06X %06X\n", label, start, end);
					fprintf(fp3, "T%06X%02X", loc_table[loc_i], obj_size[size_i]);
					printf("T%06X %02X", loc_table[loc_i], obj_size[size_i]);
					size_i++;
				}
				else {
					if (strcmp(opcode, "END") == 0) { strcpy(objcode, ""); fprintf(fp3, "\n"); printf("\n"); break; }
					//opjcode 연산
					else if (strcmp(opcode, "BASE") == 0) { base = get_sym_loc(operand); if (base == -1) { printf("base error\n"); } strcpy(objcode, ""); }
					else if (strcmp(opcode, "WORD") == 0) { 
						if (size + 3 > 30) {
							fprintf(fp3, "\nT%06X%02X%06s", loc_table[loc_i], obj_size[size_i], operand);
							printf("\nT%06X %02X %06s", loc_table[loc_i], obj_size[size_i], operand);
							size = 3; size_i++;
						}
						else {
							fprintf(fp3, "%06s", operand);
							printf(" %06s", operand);
							size += 3;
						}
					}
					else if (strcmp(opcode, "RESW") == 0) { size_i++; size = 0; }
					else if (strcmp(opcode, "RESB") == 0) { size_i++; size = 0; }
					else if (strcmp(opcode, "BYTE") == 0) { 
						temp = strtok(operand, "'");
						if (strcmp(temp, "X") == 0) {
							temp = strtok(NULL, "'");
							if (size + strlen(temp)/2 > 30 || size == 0) {
								fprintf(fp3, "\nT%06X%02X%s", loc_table[loc_i], obj_size[size_i], temp);
								printf("\nT%06X %02X %s", loc_table[loc_i], obj_size[size_i], temp);
								size = strlen(temp)/2; size_i++;
							}
							else {
								fprintf(fp3, "%s", temp);
								printf(" %s", temp);
								size += strlen(temp)/2;
							}
						}
						else if(strcmp(temp, "C") == 0){
							temp = strtok(NULL, "'");
							if (size + strlen(temp) > 30 || size == 0) {
								fprintf(fp3, "\nT%06X%02X", loc_table[loc_i], obj_size[size_i]);
								printf("\nT%06X %02X", loc_table[loc_i], obj_size[size_i]);
								for (i = 0; i < strlen(temp); i++) {
									fprintf(fp3, "%02X", (int)(temp[i]));
									printf("%02X", (int)(temp[i]));
								}
								size = strlen(temp); size_i++;
							}
							else {
								printf(" ");
								for (i = 0; i < strlen(temp); i++) {
									fprintf(fp3, "%02X", (int)(temp[i]));
									printf("%02X", (int)(temp[i]));
								}
								size += strlen(temp);
							}
						}
						else {
							return -2;
						}
					}
					else {
						op = get_opcode(opcode);
						if (op == -1) {
							//printf("error\n"); return -2;
						}
						if (plus == 1) {
							if (type == 0) { op += 3; }//label의 주소를 갖고 연산
							else if (type == 2) { op += 1; }//해당 값을 갖고 연산
							else { printf("plus error\n"); }//주어진 형식이 맞지 않음 + 와 @이가 같이 있을경우
						}
						else {
						}

						if (fm == 1) {
							if (size + fm > 30) { fprintf(fp3, "\nT%06X%02X", loc_table[loc_i], obj_size[size_i]); size = fm; size_i++; }
							else { fprintf(fp3, ""); size += fm; }
						}
						else if (fm == 2) {
							strcpy(reg1, " ");
							strcpy(reg2, " ");
							temp = strtok(operand, ",");
							strcpy(reg1, temp);
							temp = strtok(NULL, "\0");
							if (temp != NULL) { strcpy(reg2, temp); }
							temp = strtok(reg1, " ");
							strcpy(reg1, temp);
							temp = strtok(reg2, " ");
							if (temp != NULL) { strcpy(reg2, temp); }
							fprintf(fp2, "%d\t%04X\t%-30s\t%02X%1X%1X\n",row,loc_table[loc_i], temp_str, op, get_reg(reg1), get_reg(reg2));
							if (size + fm > 30 || size == 0) {
								fprintf(fp3, "\nT%06X%02X%02X%1X%1X", loc_table[loc_i], obj_size[size_i], op, get_reg(reg1), get_reg(reg2));
								printf("\nT%06X %02X %02X%1X%1X", loc_table[loc_i], obj_size[size_i], op, get_reg(reg1), get_reg(reg2));
								size = fm; size_i++;
							}
							else {
								fprintf(fp3, "%02X%1X%1X", op, get_reg(reg1), get_reg(reg2));
								printf(" %02X%1X%1X", op, get_reg(reg1), get_reg(reg2));
								size += fm;
							}
						}
						else if (fm == 3) {
							if (type == 0) { op += 3; }
							else if (type == 1) { op += 2; }
							else if (type == 2) { op += 1; }

							if (strchr(operand, ',') != NULL) {
								temp = strtok(operand, ",");
								strcpy(operand, temp);
								mid += 8;
							}
							lvalue = get_sym_loc(operand);
							//printf("%s: %05X: %05X\n", operand, lvalue, loc_table[loc_i]);
							if (lvalue == -1) { lvalue = trans_str_to_dec(operand); }
							else if (lvalue - loc_table[loc_i + 1] <= 1024) {
								mid += 2;
								lvalue = lvalue - loc_table[loc_i + 1];
							}
							else { mid += 4; lvalue = lvalue - base; }
							fprintf(fp2, "%3d\t%04X\t%-30s\t%02X%1X%03X\n",row, loc_table[loc_i], temp_str, op, mid, (lvalue & 0xfff));
							if (size + fm > 30 || size == 0) {
								fprintf(fp3, "\nT%06X%02X%02X%1X%03X", loc_table[loc_i], obj_size[size_i], op, mid, (lvalue & 0xfff));
								printf("\nT%06X %02X %02X%1X%03X", loc_table[loc_i], obj_size[size_i], op, mid, (lvalue & 0xfff));
								size = fm; size_i++;
							}
							else {
								fprintf(fp3, "%02X%1X%03X", op, mid, (lvalue & 0xfff));
								printf(" %02X%1X%03X", op, mid, (lvalue & 0xfff));
								size += fm;
							}
						}
						else if (fm == 4) {
							mid += 1;
							lvalue = get_sym_loc(operand);
							if (lvalue == -1) { lvalue = trans_str_to_dec(operand); }
							fprintf(fp2, "%cd\t%04X\t%-30s\t%02X%1X%05X\n",row, loc_table[loc_i], temp_str, op, mid, (lvalue& 0xfffff));
							if (size + fm > 30 || size == 0) {
								fprintf(fp3, "\nT%06X%02X%02X02X%1X%05X", loc_table[loc_i], obj_size[size_i], op, mid, (lvalue & 0xfffff));
								printf("\nT%06X %02X %02X%1X%05X", loc_table[loc_i], obj_size[size_i], op, mid, (lvalue & 0xfffff));
								size = fm; size_i++;
							}
							else {
								fprintf(fp3, "%02X%1X%05X", op, mid, (lvalue & 0xfffff));
								printf(" %02X%1X%05X", op, mid, (lvalue & 0xfffff));
								size += fm;
							}
						}
						else {

						}
					}
				}
			}//"."이 아니면 end
		}//"\n", "\0" end
		loc_i++;
		row += 5;
	}
	return 0;
}