#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string.h>
#include"systactic analyzer.h"
using namespace std;
typedef enum { integer } types;
typedef struct {
	char vname[17];
	char vproc[17];
	bool vkind;
	types vtype;
	int vlev;
	int vadr;
} varrecord;
typedef struct {
	char pname[17];
	types ptype;
	int plev;
	int varNum;
	int fadr;
	int ladr;
	int parameter;
	bool parameterIsDefined;
} prorecord;

varrecord currentvar;
prorecord currentpro;
varrecord varlist[1000];//��ű�������������
prorecord prolist[1000];//��Ź�������������
int varcount;//����������
int procount;//���̵�����
int num = 0;
int position = 0;
int wordposition = 0;
int linenum = 1;
FILE *input;
FILE *output;
FILE  *var;
FILE * pro;
FILE *err;
char inputword[1000][17] = { '\0' };
int kind[1000];
char tstring[21] = { '\0' };
void Finalize()
{
	for (int i = 0; i < varcount; i++)
	{
		int vkind = varlist[i].vkind ? 1 : 0;
		const char* vtype = (varlist[i].vtype == integer) ? "integer" : "";
		fprintf(var, "������: %s ��������: %s ����: %d ��������: %s �������: %d ��������λ��: %d\n", varlist[i].vname, varlist[i].vproc, vkind, vtype, varlist[i].vlev, varlist[i].vadr);
	}
	for (int i = 0; i < procount; i++)
	{
		const char* ptype = (prolist[i].ptype == integer) ? "integer" : "";
		fprintf(pro, "������: %s ��������: %s ���̲��: %d ��һ��������λ��: %d ���һ��������λ��: %d\n", prolist[i].pname, ptype, prolist[i].plev, prolist[i].fadr, prolist[i].ladr);
	}
	if (fseek(input, 0, 0) == 0)
	{
		while (!feof(input))
		{
			memset(tstring, 0, sizeof(tstring));
			fgets(tstring, 21, input);
			fputs(tstring, output);
		}
	}

	bool val;
	val = fclose(input);
	val = fclose(output);
	val = fclose(err);
	val = fclose(var);
	val = fclose(pro);
	
}
bool error(string s, const char* symbol)
{
	const char* errInfo;
	
	if(s=="nodefine")
		fprintf(err, "***LINE:%d  %s�����޶���\n", linenum, inputword[position]);
	else if(s=="redefine")
		fprintf(err, "***LINE:%d  %s�����ض���\n", linenum, inputword[position]);
	else if(s=="nomatch")
		fprintf(err, "***LINE:%d  %s������ƥ��ִ�����\n", linenum, inputword[position]);
	else if(s=="lack")
		fprintf(err, "***LINE:%d  %s��ȱ��%s\n", linenum, inputword[position], symbol);
	else if(s=="lacksemicolon")
		fprintf(err, "***LINE:%d  %s��ȱ��%s\n", linenum - 1, inputword[position - 1], symbol);
	else if(s=="nopara")
		fprintf(err, "***LINE:%d  ȱ���β�%s������\n", linenum, symbol);
	else
		errInfo = "δ֪����";
	
	return true;
}
bool isVarExisted(char* vname, char* vproc, bool vkind)
{
	for (int i = 0; i < varcount; i++)
	{
		if ((strcmp(vname, varlist[i].vname) == 0) && (strcmp(vproc, varlist[i].vproc) == 0) && (varlist[i].vkind == vkind))
			return true;
	}
	for (int i = 0; i < procount; i++)
	{
		if (strcmp(vname, prolist[i].pname) == 0)
			return true;
	}
	return false;
}
bool isProExisted(char* vname)
{
	for (int i = 0; i < varcount; i++)
	{
		if (strcmp(vname, varlist[i].vname) == 0)
			return true;
	}
	for (int i = 0; i < procount; i++)
	{
		if (strcmp(vname, prolist[i].pname) == 0)
			return true;
	}
	return false;
}
int getnextposition()
{
	int pNextToken = position + 1;
	while (strcmp(inputword[pNextToken], "EOLN") == 0)
	{
		pNextToken++;
	}
	return pNextToken;
}
bool next() {
	position++;
	wordposition = 0;
	if (strcmp(inputword[position], "EOF") == 0)
	{
		return true;
	}
	while (strcmp(inputword[position], "EOLN") == 0)
	{
		position++;
		linenum++;
	}
	return false;
}
void Program() {
	SmallProgram();
}
void Execution()
{
	if (strcmp(inputword[position], "read") == 0)
	{
		Read();
	}
	else if (strcmp(inputword[position], "write") == 0)
	{
		Write();
	}
	else if (strcmp(inputword[position], "if") == 0)
	{
		Condition();
	}
	else if (kind[position] == 10)
	{
		Assignment();
	}
	else if (strcmp(inputword[position], "end") == 0)
		return ;
	else
	{
		return;
	}
}
void _ExecutionTable()
{
	if (strcmp(inputword[position], ";") == 0)
	{
		next();
		Execution();
		_ExecutionTable();
	}
	else if (strcmp(inputword[position], "") == 0)
		return;
	else
	{
		if ((strcmp(inputword[position], "end") != 0) && (strcmp(inputword[position], "EOF") != 0))
		{
			error("lacksemicolon", ";");
			Execution();
			_ExecutionTable();
		}
		else if (strcmp(inputword[position], "EOF") == 0)
			return;
	}
}
void ExecutionTable()
{
	Execution();
	_ExecutionTable();
}
void SmallProgram() {
	if (strcmp(inputword[position], "begin") == 0)
	{
		next();
	}
	else
	{
		error("lack", "begin");
	if (strcmp(inputword[position], "integer") != 0)
		{
			next();
		}

	}
	StatementTable();
	if (strcmp(inputword[position], ";") == 0)
	{
		next();
	}
	else
	{
		error("lacksemicolon", ";");
	if ((strcmp(inputword[position], "integer") != 0) && (strcmp(inputword[position], "read") != 0) && (strcmp(inputword[position], "write") != 0) && (kind[position] != 10))
		{
			next();
		}
	}
	 ExecutionTable();
	if (strcmp(inputword[position], "end") == 0)
	{
		next();
	}
	else
	{
		error("lack", "end");
	}
}
void StatementTable()
{
	Statement();
	_StatementTable();
}
void _StatementTable()
{
	if (strcmp(inputword[position], ";") == 0 && strcmp(inputword[getnextposition()], "integer") == 0)
	{ 
		next();
		Statement();
		_StatementTable();
	}
	else
	{
		if (strcmp(inputword[position], "integer") == 0)
		{
			error("lacksemicolon", ";");
			Statement();
			_StatementTable();
		}
	}
}
void Statement()
{
	if (strcmp(inputword[position + 1], "function") == 0)
	{
		FunctionStatement();
	}
	else
	{
		VariableStatement();
	}
}
void VariableStatement()
{
	if (strcmp(inputword[position], "integer") == 0)
	{
		next();
	}
	else
	{
		error("lack", "integer");
		next();
	}
	strcpy(currentvar.vname, inputword[position]);
	strcpy(currentvar.vproc, currentpro.pname);
	if (position ==currentpro.parameter)
	{
		currentvar.vkind = true;
		currentpro.parameterIsDefined = true;
	}
	else
	{
		currentvar.vkind = false;
	}
	currentvar.vtype = integer;
	currentvar.vlev = currentpro.plev;
	currentvar.vadr = varcount;
	if (isVarExisted(inputword[position], currentpro.pname, currentvar.vkind))//������ڱ���
	{
		error("redefine", NULL);
	}
	else
	{
		if (currentpro.varNum == 0)//�����ǰ�����б�����Ϊ0����ǰ�����ǵ�ǰ���̵ĵ�һ������
		{
			currentpro.fadr = currentvar.vadr;
		}
		currentpro.ladr = currentvar.vadr;//���������һ�������ڱ������е�λ��
		currentpro.varNum++;//�����б�����++
		varlist[varcount] = currentvar;//��ǰ��������var����
		varcount++;//������++
	}
	Variable();
}
bool Variable()
{
	return Identifier();
}
bool Identifier()
{
	if (kind[position] == 10)
	{
		next();
		return true;
	}
	return false;
}

void FunctionStatement()
{
	prorecord proBackup = currentpro;//���ݵ�ǰ���̣���ƥ����G���̺�ָ�
	if (strcmp(inputword[position], "integer") == 0)
	{
		next();
	}
	else
	{
		error("lack", "integer");
		if (strcmp(inputword[position], "function") != 0)
		{
			next();
		}
	}
	if (strcmp(inputword[position], "function") == 0)
	{
		next();
	}
	else
	{
		error("lack", "function");
		if (kind[position] != 10)
		{
			next();
		}
	}
	strcpy(currentpro.pname, inputword[position]);
	currentpro.ptype = integer;
	currentpro.plev++;
	currentpro.varNum = 0;
	currentpro.parameterIsDefined = false;
	if (isProExisted(inputword[position]))
	{
		error("redefine", NULL);
	}
	Identifier();
	if (strcmp(inputword[position], "(") == 0)
	{
		next();
	}
	else
	{
		error("lack", "(");
		if (kind[position] != 10)
		{
			next();
		}
	}
	if (Parameter())
	{
		currentpro.parameterIsDefined = true;
		currentpro.parameter = position;
	}
	if (strcmp(inputword[position], ")") == 0)
	{
		next();
	}
	else
	{
		error("lack", ")");
		if (strcmp(inputword[position], ";") != 0)
		{
			next();
		}
	}
	if (strcmp(inputword[position], ";") == 0)
	{
		next();
	}
	else
	{
		error("lacksemicolon", ";");
		if (strcmp(inputword[position], "begin") != 0)
		{
			next();
		}
	}
	FunctionBody();
	currentpro = proBackup;//ƥ����G���̺�ָ�ԭ����
}
bool Parameter()
{
	 return Variable();
}
void FunctionBody()
{
	if (strcmp(inputword[position], "begin") == 0)
	{
		next();
	}
	else
	{
		error("lack", "begin");
		if (strcmp(inputword[position], "integer") != 0)
		{
			next();
		}
	}
	StatementTable();
	if (!currentpro.parameterIsDefined)
	{
		error("nopara", inputword[position-1]);
	}
	prolist[procount] = currentpro;//�������������J()��������currentPro����pro��������ΪM�п��ܻ�ʹ�õ�ǰ����(�ݹ�)
	procount++;
	if (strcmp(inputword[position], ";") == 0)
	{
		next();
	}
	else
	{
		error("lacksemicolon", ";");
		if ((strcmp(inputword[position], "if") != 0) && (strcmp(inputword[position], "read") != 0) && (strcmp(inputword[position], "write") != 0) && (kind[position] != 10))
		{
			next();
		}
	}
	ExecutionTable();
	if (strcmp(inputword[position], "end") == 0)
	{
		next();
	}
	else
	{
		error("lack", "end");
		if ((strcmp(inputword[position], ";") != 0) && (strcmp(inputword[position], "end") != 0))
		{
			next();
		}
	}
}
void Read()
{
	if (strcmp(inputword[position], "read") == 0)
	{
		next();
	}
	else
	{
		error("lack", "read");
		if (strcmp(inputword[position], "(") != 0)
		{
			next();
		}
	}
	if (strcmp(inputword[position], "(") == 0)
	{
		next();
	}
	else
	{
		error("lack", "(");
		if (kind[position] != 10)
		{
			next();
		}
	}
	if (!isVarExisted(inputword[position], currentpro.pname, false) && !isVarExisted(inputword[position], currentpro.pname, true))
	{
		error("nodefine", NULL);
	}
	Variable();
	
	if (strcmp(inputword[position], ")") == 0)
	{
		next();
	}
	else
	{
		error("lack", ")");
		if ((strcmp(inputword[position], ";") != 0) && (strcmp(inputword[position], "end") != 0))
		{
			next();
		}
	}
}
void Write()
{
	if (strcmp(inputword[position], "write") == 0)
	{
		next();
	}
	else
	{
		error("lack", "write");
		if (strcmp(inputword[position], "(") != 0)
		{
			next();
		}
	}
	if (strcmp(inputword[position], "(") == 0)
	{
		next();
	}
	else
	{
		error("lack", "(");
		if (kind[position] != 10)
		{
			next();
		}
	}
	if (!isVarExisted(inputword[position], currentpro.pname, false) && !isVarExisted(inputword[position], currentpro.pname, true))
	{
		error("nodefine", NULL);
	}
	Variable();
	if (strcmp(inputword[position], ")") == 0)
	{
		next();
	}
	else
	{
		error("lack", ")");
		if ((strcmp(inputword[position], ";") != 0) && (strcmp(inputword[position], "end") != 0))
		{
			next();
		}
	}
}
void Assignment()
{
	if (!isVarExisted(inputword[position], currentpro.pname, false) && !isVarExisted(inputword[position], currentpro.pname, true) && !isProExisted(inputword[position]))
	{
		error("nodefine", NULL);
	}
	Variable();
	if (strcmp(inputword[position], ":=") == 0)
	{
		next();
	}
	else
	{
		error("lack", ":=");
		if ((kind[position] != 10) && (kind[position] != 11))
		{
			next();
		}
	}
	ArithmeticExpression();
}
void ArithmeticExpression()
{
	Item();
	_ArithmeticExpression();
}
void _ArithmeticExpression()
{
	if (strcmp(inputword[position], "-") == 0)
	{
		next();
		Item();
		_ArithmeticExpression();
	}
	/*else
	{
		if ((kind[position] == 10) || (kind[position] == 11))
		{
			Item();
			_ArithmeticExpression();
		}
	}*/
}
void Item()
{
	Factor();
	_Item();
}
void _Item()
{
	if (strcmp(inputword[position], "*") == 0)
	{
		next();
		Factor();
		_Item();
	}
	/*else
	{
		if ((kind[position] == 10) || (kind[position] == 11))
		{
			Factor();
			_Item();
		}
	}*/
}
void Factor()
{
	if (inputword[position][wordposition] >= '0' && inputword[position][wordposition] <= '9')
	{
		Constant();
	}
	else if (strcmp(inputword[getnextposition()], "(") == 0)
	{
		FunctionUse();
	}
	else
	{
		if (!isVarExisted(inputword[position], currentpro.pname, false) && !isVarExisted(inputword[position], currentpro.pname, true))
		{
			error("nodefine", NULL);
		}
		Variable();
	}
}
void Constant()
{
	if (kind[position] == 11)
	{
		next();
	}
}
void Condition()
{
	if (strcmp(inputword[position], "if") == 0)
	{
		next();
	}
	else
	{
		error("lack", "if");
		if ((kind[position] != 10) && (kind[position] != 11))
		{
			next();
		}
	}
	ConditionExpression();
	if (strcmp(inputword[position], "then") == 0)
	{
		next();
	}
	else
	{
		error("lack", "then");
		if ((strcmp(inputword[position], "integer") != 0) && (strcmp(inputword[position], "read") != 0) && (strcmp(inputword[position], "write") != 0) && (kind[position] != 10))
		{
			next();
		}
	}
	Execution();
	if (strcmp(inputword[position], "else") == 0)
	{
		next();
	}
	else
	{
		error("lack", "else");
		if ((strcmp(inputword[position], "integer") != 0) && (strcmp(inputword[position], "read") != 0) && (strcmp(inputword[position], "write") != 0) && (kind[position] != 10))
		{
			next();
		}
	}
	Execution();
}
void ConditionExpression()
{
	ArithmeticExpression();
	RelationOperator();
	ArithmeticExpression();
}
void RelationOperator()
{
	if (strcmp(inputword[position], "<") == 0 || strcmp(inputword[position], "<=") == 0 || strcmp(inputword[position], ">") == 0 || strcmp(inputword[position], ">=") == 0 || strcmp(inputword[position], "=") == 0 || strcmp(inputword[position], "<>") == 0)
	{
		next();
	}
	else
	{
		 error("lack", "��ϵ�����");
		if ((kind[position] != 10) && (kind[position] != 11))
		{
			next();
		}
	}
}
void FunctionUse()
{
	if (!isProExisted(inputword[position]))
	{
		error("nodefine", NULL);
	}
	Identifier();
	if (strcmp(inputword[position], "(") == 0)
	{
		next();
	}
	else
	{
		error("lack", "(");
		if ((kind[position] != 10) && (kind[position] != 11))
		{
			next();
		}
	}
	ArithmeticExpression();
	if (strcmp(inputword[position], ")") == 0)
	{
		next();
	}
	else
	{
		error("lack", ")");
		if ((strcmp(inputword[position], "-") != 0) && (strcmp(inputword[position], "*") != 0) && (strcmp(inputword[position], ";") != 0) && (strcmp(inputword[position], "end")))
		{
			next();
		}
	}
}






int main()
{
	input = fopen("output.dyd", "r");
	if (input == NULL)
	{
		cout << "��ȡ�ļ�ʧ��" << endl;
		return 1;
	}
	err = fopen("error.err", "w");
	output = fopen("output.dys", "ab");
	var = fopen("var.var", "w");
	pro = fopen("pro.pro", "w");
	char tempstring[21] = { '\0' };
	char* kindstring;
	char *startstring;
	char tstring[17] = { '\0' };
	while (!feof(input))
	{
		   memset(tempstring, 0, sizeof(tempstring));
		   fgets(tempstring, 21, input);
			kindstring = strrchr(tempstring, ' ');
			if (kindstring == NULL)
				continue;
			kind[num] = atoi(kindstring + 1);
			strncpy(tstring, tempstring, 16);
			startstring = strrchr(tstring, ' ');
			strcpy(inputword[num], startstring + 1);
			num++;	
	}
	Program();
	Finalize();
	return 0;
}