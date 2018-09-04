//#include<stdio.h>
#include "20161624.h"
int split_inst(char* str, char** factor) {
	char* inst;
	char* temp;
	int i = 0;
	for (i = 0; i < 3; i++) {
		factor[i] = NULL;
	}
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '\t') {
			str[i] = ' ';
		}
	}
	inst = strtok(str, "\n");
	inst = strtok(str, " ");
	temp = strtok(NULL, "\n");
	i = 0;
	if (temp != NULL) {
		factor[0] = strtok(temp, ",");
		while (factor[i] != NULL) {
			i++;
			factor[i] = strtok(NULL, ",");
		}
		for (i = 0; i < 3; i++) {
			if (factor[i] != NULL) {
				factor[i] = strtok(factor[i], " ");
				if (strtok(NULL, " ")) {
					return -2;
				}
			}
		}
	}
	/*
	for (i = 0; i < 3; i++) {
		printf(".%s.\n", factor[i]);
	}
	*/
	if (!strcmp(inst, "quit") || !strcmp(inst, "q")) { if (factor[0] != NULL) { return -2; } else { return 0; } }
	else if (!strcmp(inst, "help") || !strcmp(inst, "h")) { if (factor[0] != NULL) { return -2; } else { return 1; } }
	else if (!strcmp(inst, "dir") || !strcmp(inst, "d")) { if (factor[0] != NULL) { return -2; } else { return 2; } }
	else if (!strcmp(inst, "history") || !strcmp(inst, "hi")) { if (factor[0] != NULL) { return -2; } else { return 3; } }
	else if (!strcmp(inst, "dump") || !strcmp(inst, "du")) { if (factor[2] != NULL) { return -2; } else { return 4; } }
	else if (!strcmp(inst, "edit") || !strcmp(inst, "e")) { if (factor[2] != NULL) { return -2; } else { return 5; } }
	else if (!strcmp(inst,"fill") || !strcmp(inst,"f")) { return 6; }
	else if (!strcmp(inst, "reset")) { if (factor[0] != NULL) { return -2; } else { return 7; } }
	else if (!strcmp(inst, "opcode")) { if (factor[1] != NULL) { return -2; } else { return 8; } }
	else if (!strcmp(inst, "opcodelist")) { if (factor[0] != NULL) { return -2; } else { return 9; } }
	else if (!strcmp(inst,"type")) { if (factor[0] == NULL || factor[1] != NULL) { return -2; } else { return 10; } }
	else if (!strcmp(inst,"assemble")) { if (factor[0] == NULL || factor[1] != NULL) { return -2; } else { return 11; } }
	else if (!strcmp(inst, "symbol")) { if (factor[0] != NULL) { return -2; } else { return 12; } }
	else if (!strcmp(inst,"progaddr")) { return 13; }
	else if (!strcmp(inst,"loader")) { return 14; }
	else if (!strcmp(inst,"run")) { return 15; }
	else if (!strcmp(inst,"bp")) { return 16; }
	else { return -1; }
}
int trans_str_to_int(char* str) {
	int i = 0, mul = 1, sum = 0;
	for (i = strlen(str) - 1; i >= 0; i--) {
		if ((int)(str[i]) >= 48 && (int)(str[i]) <= 57) {
			sum = sum + ((int)(str[i]) - 48)*mul;
		}
		else if ((int)(str[i]) >= 65 && (int)(str[i]) <= 70) {
			sum = sum + ((int)(str[i]) - 55)*mul;
		}
		else {
			return -1;
		}
		mul = mul * 16;
	}
	//if (sum >= 128) {
	//	return -1;
	//}
	return sum;
}
int trans_str_to_dec(char* str) {
	int i = 0, mul = 1, sum = 0;
	for (i = strlen(str) - 1; i >= 0; i--) {
		if ((int)(str[i]) >= 48 && (int)(str[i]) <= 57) {
			sum = sum + ((int)(str[i]) - 48)*mul;
		}
		else {
			return -1;
		}
		mul = mul * 10;
	}
	//if (sum >= 128) {
	//	return -1;
	//}
	return sum;
}
int help(void) {
	fprintf(stderr, "\n");
	fprintf(stderr, "h[elp]\n");
	fprintf(stderr, "d[ir]\n");
	fprintf(stderr, "q[uit]\n");
	fprintf(stderr, "hi[story]\n");
	fprintf(stderr, "du[mp] [start, end]\n");
	fprintf(stderr, "e[dit] address, value\n");
	fprintf(stderr, "f[ill] start, end, value\n");
	fprintf(stderr, "reset\n");
	fprintf(stderr, "opcode mnemonic\n");
	fprintf(stderr, "opcodelist\n");
	fprintf(stderr, "assemble filename\n");
	fprintf(stderr, "type filename\n");
	fprintf(stderr, "symbol\n");
	fprintf(stderr, "progaddr [address]\n");
	fprintf(stderr, "loader [object filename1][object filename2][...]\n");
	fprintf(stderr, "run\n");
	fprintf(stderr, "bp\n");
	fprintf(stderr, "\n");
	return 0;
}
int diretory(void) {//header파일이 없는관계로 생략
	/*
	DIR* dp = NULL;//디렉토리 포인터
	struct dirent *dir_entry = NULL;//dirent 구조체를 이용하여 파일이름을 출력한다.
	struct stat buf;
	if ((dp = opendir(".")) == NULL) {//디렉토리를 open할 때 문제가 있을시 >출력.
		printf("현재 디렉토리를 열수 없습니다.\n");
		return -1;
	}
	printf("\n");
	while (dir_entry = readdir(dp)) {//디렉토리 엔트리를 읽어들인다.
		lstat(dir_entry->d_name, &buf);//파일의 정보를 읽어들인다.
		if (S_ISDIR(buf.st_mode)) {//파일형식이 디렉토리이면 실행. 주어>진 매크로를 활용.
			printf("%s/\t", dir_entry->d_name);
		}
		else if (S_ISREG(buf.st_mode)) {//파일형식이 일반파일이면 실행. 주어진 매크로를 활용.
			printf("%s*\t", dir_entry->d_name);
		}
	}
	printf("\n");
	printf("\n");
	closedir(dp);//디렉토리를 닫는다.
	return 0;
	*/
}
int history(char* str) {
	historytable* temp;
	historytable* target;
	temp = (historytable*)malloc(sizeof(historytable));
	strcpy(temp->str, str);
	temp->link = NULL;
	if (his_head == NULL) {
		his_head = temp;
	}
	else {
		target = his_head;
		while (target->link != NULL) {
			target = target->link;
		}
		target->link = temp;
	}
	return 0;
}
void history_print(void) {
	int i = 0;
	historytable* target = his_head;
	fprintf(stderr, "\n");
	while (target) {
		fprintf(stderr, "%d\t%s", i, target->str);
		target = target->link;
		i++;
	}
	fprintf(stderr, "\n");
}
void history_del(void) {
	historytable* target = his_head;
	historytable* temp;
	if (target != NULL) {
		while (target == NULL) {
			temp = target;
			target = target->link;
			free(temp);
		}
	}
}
int dump(char* start, char* end) {
	int start_i, end_i, i = 0, j = 0;
	int start_loc, end_loc;
	if (start == NULL) {
		start_i = dump_addr;
	}
	else {
		start_i = trans_str_to_int(start);
		if (start_i == -1) { return - 2; }
	}
	if (end == NULL) {
		end_i = start_i + 159;
		if (end_i > MAX_MEM_SIZE) end_i = MAX_MEM_SIZE;
	}
	else {
		end_i = trans_str_to_int(end);
		if (end_i == -1) { return -2; }
	}
	if (start_i > end_i && end_i != -1) {
		return -1;
	}
	start_loc = start_i / MAX_MEM_COL;
	end_loc = end_i / MAX_MEM_COL;
	for (i = start_i / MAX_MEM_COL; i <= end_i / MAX_MEM_COL; i++) {
		fprintf(stderr, "%05X", i*MAX_MEM_COL);
		for (j = 0; j < MAX_MEM_COL; j++) {
			if (i * MAX_MEM_COL + j >= start_i && i * MAX_MEM_COL + j <= end_i) {
				fprintf(stderr, " %02X", virtual_mem[i][j]);
			}
			else { fprintf(stderr, "   "); }
		}
		fprintf(stderr, " ;");
		for (j = 0; j < MAX_MEM_COL; j++) {
			if ((i * MAX_MEM_COL + j) >= start_i && (i * MAX_MEM_COL + j) <= end_i) {
				if (virtual_mem[i][j] == NULL) { fprintf(stderr, "."); }
				else { fprintf(stderr, "%c", virtual_mem[i][j]); }
			}
			else if (virtual_mem[i][j] == NULL) { fprintf(stderr, "."); }
			else { fprintf(stderr, "."); }
		}
		fprintf(stderr, "\n");
	}
	dump_addr = end_i + 1;
	return 0;
}
int edit(char* address, char* value) {
	int address_i = 0, value_i = 0;
	if (address == NULL || value == NULL) { return -2; }
	address_i = trans_str_to_int(address);
	value_i = trans_str_to_int(value);
	if (address_i == -1 || value_i == -1 || value_i >= 128) { return -2; }
	virtual_mem[address_i / 16][address_i % 16] = value_i;
	return 0;
}
int fill(char* start, char* end, char* value) {
	int start_i = 0, end_i = 0, value_i = 0, i;
	if (start == NULL || end == NULL || value == NULL) { return -2; }
	start_i = trans_str_to_int(start);
	end_i = trans_str_to_int(end);
	value_i = trans_str_to_int(value);
	if (start_i == -1 || end_i == -1 || value_i == -1 || value_i >= 128) { return -2; }
	for (i = start_i; i <= end_i; i++) {
		virtual_mem[i / 16][i % 16] = value_i;
	}
	return 0;
}
int reset(void) {
	int i, j;
	for (i = 0; i < MAX_MEM_ROW; i++) {
		for (j = 0; j < MAX_MEM_COL; j++) {
			virtual_mem[i][j] = NULL;
		}
	}
	return 0;
}
int opcode(char* mnemonic) {
	int key;
	//opcodetable* target;
	if (mnemonic == NULL) {
		return -2;
	}
	/*
	key = hash_func(mnemonic);
	target = op_table[key].link;
	while (1) {
		if (strcmp(target->name, mnemonic) == 0) {
			fprintf(stderr, "opcode is %s\n", target->num);
			return 0;
		}
		if (target->link == NULL) { break; }
		target = target->link;
	}*/
	key = get_opcode(mnemonic);
	if (key != -1) {
		fprintf(stderr, "opcode is %s\n", key);
		return 0;
	}
	else {
		return -2;
	}
}
int opcodelist(void) {
	int i;
	opcodetable* target;
	for (i = 0; i < 20; i++) {
		fprintf(stderr, "%d : ", i);
		target = op_table[i].link;
		while (target != NULL) {
			fprintf(stderr, "[%s,%s]", target->name, target->num);
			if (target->link != NULL) { fprintf(stderr, " -> "); }
			target = target->link;
		}
		fprintf(stderr, "\n");
	}
}
int type(char* filename) {
	FILE* fp;
	char str[100];
	if (!(fp = fopen(filename, "r"))) {
		fputs("해당파일 오픈 실패\n", stderr);
		return -2;
	}
	fprintf(stderr, "\n");
	while (!feof(fp)) {
		fgets(str, sizeof(str), fp);
		fprintf(stderr, "\t%s", str);
	}
	fprintf(stderr, "\n\n");
	return 0;
}
int assemble(char* filename) {
	FILE* fp;
	FILE* fp2;
	FILE* fp3;
	char* name;
	char* t;
	char temp[30];
	char str[30];
	strcpy(temp, filename);
	name = strtok(temp, ".");
	t = strtok(NULL, " ");
	if (strcmp(t, "asm") != 0) {
		return -2;
	}
	if (!(fp = fopen(filename, "r"))) {
		fputs("해당파일 오픈 실패\n", stderr);
		return -2;
	}
	pass1(fp);
	fclose(fp); 

	if (!(fp = fopen(filename, "r"))) {
		fputs("해당파일 오픈 실패\n", stderr);
		return -2;
	}
	strcat(name, ".lst");
	if (!(fp2 = fopen(name, "w"))) {
		fputs("해당파일 오픈 실패\n", stderr);
		return -2;
	}
	name = strtok(name, ".");
	strcat(name, ".obj");
	if (!(fp3 = fopen(name, "w"))) {
		fputs("해당파일 오픈 실패\n", stderr);
		return -2;
	}
	pass2(fp, fp2, fp3);
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	return 0;
}
int symbol(void) {
	int i;
	symboltable* target;
	fprintf(stderr, "\n");
	for (i = 0; i < 20; i++) {
		target = sym_table[i].link;
		while (target != NULL) {
			fprintf(stderr, "\t%s\t%04X\n", target->label, target->loc);
			target = target->link;
		}
	}
	fprintf(stderr, "\n");
	return 0;
}
int main_board(void) {
}

int main() {
	char str[MAX_STRING_SIZE];
	char** factor;
	char* copy;
	int type_i;
	int i, j, result = 0;
	factor = (char**)malloc(sizeof(char*)*3);
	for (i = 0; i < 3; i++) {
		factor[i] = (char*)malloc(sizeof(char) * MAX_STRING_SIZE);
	 }
	virtual_mem = (char**)malloc(sizeof(char*) * MAX_MEM_ROW);
	for (i = 0; i < MAX_MEM_ROW; i++) {
		virtual_mem[i] = (char*)malloc(sizeof(char)*MAX_MEM_COL);
		for (j = 0; j < MAX_MEM_COL; j++) {
			virtual_mem[i][j] = 0;
		}
	}
	op_table = (opcodetable*)malloc(sizeof(opcodetable) * MAX_HASH_ROW);
	sym_table = (symboltable*)malloc(sizeof(symboltable) * MAX_HASH_ROW);
	for (i = 0; i < MAX_HASH_ROW; i++) {
		op_table[i].link = NULL;
		sym_table[i].link = NULL;
	}
	make_opcode_table();
	while (1) {
		type_i = 0;
		result = 0;
		fprintf(stdout, "sicsim> ");
		fgets(str, sizeof(str), stdin);
		copy = (char*)malloc(sizeof(char)*strlen(str));
		strcpy(copy, str);
		type_i = split_inst(str, factor); 
		if (!type_i) { break; }
		else if (type_i == -1) {
			fprintf(stderr, "\nERROR: Can't find your shell instruction!\n\n");
		}
		else if (type_i == -2) {
			fprintf(stderr, "\nERROR: Factor type is error!\n\n");
		}
		else {
			switch (type_i) {
			case 1: { result = help();  break; }
			case 2: { result = diretory(); break; }
			case 3: { history(copy); history_print();  break; }
			case 4: { result = dump(factor[0], factor[1]); break; }
			case 5: { result = edit(factor[0], factor[1]); break; }
			case 6: { result = fill(factor[0], factor[1], factor[2]); break; }
			case 7: { result = reset(); break; }
			case 8: { result = opcode(factor[0]); break; }
			case 9: { result = opcodelist(); break; }
			case 10: { result = type(factor[0]); break; }
			case 11: { result = assemble(factor[0]); break; }
			case 12: { result = symbol(); break; }
			case 13: {break; }
			case 14: {break; }
			case 15: {break; }
			case 16: {break; }
			}
			if (result == -2) {
				fprintf(stderr, "\nERROR: Factor type is error!\n\n");
			}
			else if(type_i != 3){
				history(copy);
			}
		}
	}
	history_del();
	for (i = 0; i < MAX_MEM_ROW; i++) {
		free(virtual_mem[i]);
	}
	free(virtual_mem);
	for (i = 0; i < 3; i++) {
		free(factor[i]);
	}
	//free(factor);

	return 0;
}
