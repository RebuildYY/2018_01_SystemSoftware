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
//help�� ���õ� �ϵ��� �����ϸ�, ��밡���� shell ��ɾ ����Ѵ�.
int directory(void);
//dir�� ���õ� �ϵ��� �����ϸ�, ���� ���丮�� �ִ� ���ϵ��� ����Ѵ�.
int history(char* str);
//history�� ���õ� �ϵ��� �����ϸ�, ���ݱ��� ���� �ùٸ� ������ ��ɾ�� ���ڵ��� �����Ѵ�.
//linked list�� ����ϸ�,�տ� num�� �ٿ� ����Ѵ�.
int dump(char* start, char* end);
//dump�� ���õ� �ϵ��� �����ϸ�, �Ҵ�� �޸��� ������ ����Ѵ�.
//���ڷ� start, end�� ���� �� �ִ�.
//���ڰ� ���� ���, �⺻������ 10����(160��)�� ����Ѵ�. ���Ŀ� dump�� ���ϰ�� �̾ �� �ںκ��� ����Ѵ�.
//start�� �ִ� ���, start�� ���� 10������ ����Ѵ�.
//start, end�� ���� ���, start���� end���� ����Ѵ�.
//start > end�� ���, ����ó���Ѵ�.
//�ּҸ� �Ѿ ��� �ּ��� ��(0xFFFFF)������ ����Ѵ�.
//dump�� �������� ��µ� ������ address�� ���ο� �����ϰ� �ִ�.
//dump ��½� binary check�� �Ͽ� exception erroró���Ѵ�.
int edit(char* address, char* value);
//address, value�� ���ڷ� ������, �ش� address�� ���� value������ �����Ѵ�.
//address, value�� 16������ ���̴�.
int fill(char* start, char* end, char* value);
//start, end, value�� ���ڷ� ������, start���� end���� value�� ������ ä���.
//start, end, value�� 16������ ���̴�.
int reset(void);
//memory�� ���� ��� 00���� �Ѵ�. 00�� 16�����̴�.
int opcode(char* mnomonic);
//mnomonic�� ���ڷ� ������, �̴� char*�δ�.
//�ش��ϴ� ��ɾ��� opcode�� ����Ѵ�.(16����)
int opcodelist(void);
//Hash Table�� ������� �Ͽ�, hashtable�� ����� opcode�� ����Ѵ�.
int assemble();
//filename�� ���ڷ� ������, filename�� �ش��ϴ� �ҽ� ������ �о object���ϰ� ������ ������ �����.
//�ҽ� ������ Ȯ���ڴ� .asm�̴�.
//������ ������ ���ϸ��� �ҽ����ϰ� �����ϰ� Ȯ���ڴ� .lst�̴�.
//object ������ ���ϸ��� �ҽ����ϰ� �����ϰ� Ȯ���ڴ� .obj�̴�.
//�ҽ����Ͽ� ������ �����ϴ� ���, ������ ���ϰ� object������ �������� �ʰ�, ���� ������ ȭ�鿡 ����Ѵ�.
//���� �߻��� �ٷ� ����� ����ȴ�.
//������ ������ ������� ���ؼ� � ���ο��� ������ �߻��ߴ����� ����Ѵ�.
int type(char* filename);
//filename�� ���ڷ� ������, ������ ���� ���͸����� �о ȭ�鿡 ����Ѵ�.
//���� ���丮�� �������� ���� ��� �����޼����� ����Ѵ�.
int symbol(void);
//assemble�������� ������ symbol table�� ȭ�鿡 ����Ѵ�.
//symbol�� ����� symbol�� �������� ������������ ������ �Ǿ�� �Ѵ�.
//���� : (\t)RETADR(\t)0030
int progaddr();
//address�� ���ڷ� ������, loader �Ǵ� rum ��ɾ ������ �� �����ϴ� �ּҸ� �����մϴ�.
//sicsim�� ���۵Ǹ� default�� progaddr�� 0x00 �ּҷ� �����ȴ�.
int loader();
//object filename���� �ִ� 3������ ���ڷ� ���� �� �ִ�.
//object������ �о linking�۾��� ���� ��, ���� �޸�(1M)�� �� ����� ����Ѵ�.(������ pass1, pass2)
//loader�� �������̸�, load map�� ȭ�鿡 ����Ѵ�.
//���ΰ� ������ ���, ���� ������ ȭ�鿡 ��µȴ�.
int run();
//loader ��ɾ��� �������� �޸𸮿� load�� ���α׷��� �����Ѵ�.
//progaddr ��ɾ�� ������ �ּҺ��� ����ȴ�.
//���� ����ν� register ���¸� ȭ�鿡 ����Ѵ�.(A, X, L, PC, B, S, T)
//break point���� ����ǰ� Breakpoint�� ������ ���α׷� ������ ����ȴ�.
int bp();
//break point�� ���õ� ���� ó���Ѵ�.
//bp�� ������ ��� ��� break point�� ����Ѵ�.
//+[address]�̸� �ش� �ּҿ� break point�� �����Ѵ�.
//+"clear"�̸� ��� break point�� �����Ѵ�.
int split_inst(char* str, char** factor);
// str(�Է¹��� ����)�� ������ factor(����)��� ������ ��ȯ�Ѵ�.
//factor�� �迭�� �ִ� 3������ ������ �� �ְ� �����.
//�ش��ϴ� ��ɾ �����ϴ� ��ȣ�� return�Ѵ�.
int get_opcode(char* mnemonic);
int get_sym_loc(char* label);
int pass1(FILE* fp);
int pass2(FILE* fp, FILE* fp2, FILE* fp3);
int load_pass1();
int load_pass2();
int trans_str_to_int(char* str);
int trans_str_to_dec(char* str);
int start, end;
//16���� ���ڿ��� 10������ ���������� �ٲپ��ش�.
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
		fputs("opcde.txt���� ���� ����\n", stderr);
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
				//���� opcode�� ���� locctr���� ���
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
									fprintf(stderr, "��ġ�� label: %s\n", label);
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
			}//"."�� �ƴϸ� end
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
				else {//���ڰ� 2��
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
			
				//���� opcode�� ���� locctr���� ���
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
					//opjcode ����
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
							if (type == 0) { op += 3; }//label�� �ּҸ� ���� ����
							else if (type == 2) { op += 1; }//�ش� ���� ���� ����
							else { printf("plus error\n"); }//�־��� ������ ���� ���� + �� @�̰� ���� �������
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
			}//"."�� �ƴϸ� end
		}//"\n", "\0" end
		loc_i++;
		row += 5;
	}
	return 0;
}