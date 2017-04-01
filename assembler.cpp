
/*Author:Shoeb Patel*/
/*Assembler for 20-bit SIC/XE relocatable program with Symbol Defining Statements */

#include <bits/stdc++.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;
using std::ios;

/*Hex and diectives table*/
char hexcode[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
string assembler_dir[]={"BASE","BYTE","WORD","RESW","RESB","EQU"};

/*Assembler Class*/

class assembler{
	
	const char* path;
	fstream file;
	ofstream output_stream;
	unordered_map<string,string> mnemonics;
	set<string> assembler_dirc;
	unordered_map<string ,string> optab;
	map<string,string> symtab;
	vector<string> code;
	map<int,string> intermediate;	
	vector<string> machine_code;
	vector<string> rel_record;
	string output;
/*Assembly language mneomonics*/
	void create_mnemonics(){
        mnemonics[(string)"ADD"]=(string)"18";
		mnemonics[(string)"ADDR"]=(string)"58";
		mnemonics[(string)"CLEAR"]=(string)"B4";
		mnemonics[(string)"COMP"]=(string)"28";
		mnemonics[(string)"COMPR"]=(string)"A0";
		mnemonics[(string)"J"]=(string)"3C";
		mnemonics[(string)"JEQ"]=(string)"30";
		mnemonics[(string)"JLT"]=(string)"38";
		mnemonics[(string)"JSUB"]=(string)"48";
		mnemonics[(string)"LDA"]=(string)"00";
		mnemonics[(string)"LDB"]=(string)"68";
		mnemonics[(string)"LDCH"]=(string)"50";
		mnemonics[(string)"LDT"]=(string)"74";
		mnemonics[(string)"RD"]=(string)"D8";
		mnemonics[(string)"RSUB"]=(string)"4C";
		mnemonics[(string)"STA"]=(string)"0C";
		mnemonics[(string)"STCH"]=(string)"54";
		mnemonics[(string)"STL"]=(string)"14";
		mnemonics[(string)"STX"]=(string)"10";
		mnemonics[(string)"TD"]=(string)"E0";
		mnemonics[(string)"TIXR"]=(string)"B8";
		mnemonics[(string)"WD"]=(string)"DC";
		mnemonics[(string)"A"]=(string)"0";
		mnemonics[(string)"X"]=(string)"1";
		mnemonics[(string)"S"]=(string)"2";
		mnemonics[(string)"T"]=(string)"5";
	}
/*Creating Assembler Directives*/
	void create_directives(){
		for(int i=0;i<6;i++)
			assembler_dirc.insert(assembler_dir[i]);
	}
/*Creating file stream*/
	void parse(){
		file.open(path);
		getcode();

	}
/*Evaluating value of numeric string*/
	int string_to_int(string a){
		int ans=0;
		for(int i=0;i<a.size();i++){
			ans*=10;
			ans+=(a[i]-'0');
		}
		return ans;
	}
/*Generating String from integer*/
	string int_to_string(int a){
		string ans="";
		if(a==0){
			ans="0000";
			return ans;
		}
		while(a){
			ans.insert(ans.begin(),a%10+'0');
			a/=10;
		}
		return ans;
	}
/*4-Digit Integer to Hexadecimal conversion*/
	string int_to_hex(int a){
		string ans="";
		int i=4;
		while(i--){
			ans.insert(ans.begin(),hexcode[a%16]);
			a/=16;
		}
		return ans;
	}
/*Parsing Assembly Code from External file to assemblers string data member*/
	void getcode(){
		while(!file.eof()){
			string temp="";
			char c;
			file.get(c);
			while(c!='\n' && !file.eof()){
				temp.push_back(c);
				file.get(c);
			}
			code.push_back(temp);
		}

	}
/*Parsing Label(if present) from statement*/
	string getlabel(string line){
		string temp="";
		for(int i=0;line[i]!=' ';i++){
			temp.push_back(line[i]);
		}
		return temp;
	}
/*Parsing operator field of statement*/
	string getopr(string line){
		string temp="";
			for(int j=11;line[j]!=' ' && j<line.size();j++){
				temp.push_back(line[j]);
				}
		return temp;	

	}
/*Parsing operand value of statement*/
	string getoperand(string line){
		string temp="";
		for(int i=20;i<line.size();i++){
			temp.push_back(line[i]);
		}
		return temp;
	}
/*Generating OPCODE symbol*/
	void generate_optab(){
		for(int i=0;i<code.size();i++){
			string temp="";
			for(int j=11;code[i][j]!=' ' && j<code[i].size();j++){
				temp.push_back(code[i][j]);
				}
			unordered_map<string,string> :: iterator k=mnemonics.find(temp);
			if(mnemonics.find(temp)!=mnemonics.end()){
				optab.insert(make_pair(string(temp),string(mnemonics[temp])));
			}
			
		}
		optab.insert(make_pair((string)"A",(string)"0"));

		optab.insert(make_pair((string)"X",(string)"1"));

		optab.insert(make_pair((string)"S",(string)"4"));

		optab.insert(make_pair((string)"T",(string)"5"));
	}
/*Assigning addresses to Statements*/
	void assign_location(){
		int locctr;
		int ctr=0;
		locctr=string_to_int(getoperand(code[0]));
		string loc=int_to_hex(locctr);
		intermediate.insert(make_pair(ctr,loc));
		intermediate.insert(make_pair(++ctr,loc));
		for(int i=2;i<code.size();i++){
			string cur_opr=getopr(code[i]);
			string pre_opr=getopr(code[i-1]);
			if(assembler_dirc.find(pre_opr)!=assembler_dirc.end()){
				if(pre_opr=="RESW")
					locctr+=3;
				if(pre_opr=="BYTE"){
					if(code[i-1][20]=='C')
						locctr+=3;
					if(code[i-1][20]=='X')
						locctr++;
				}
				if(pre_opr=="RESB")
					locctr+=string_to_int(getoperand(code[i-1]));
			}
			else if(code[i-1][10]=='+'){
				locctr+=4;
			}
			else if(pre_opr=="CLEAR" || pre_opr=="COMPR" || pre_opr=="TIXR"){
				locctr+=2;
			}
			else{
				locctr+=3;
			}

			if(cur_opr=="BASE"){
				loc="____";
			}
			else{
				loc=int_to_hex(locctr);
			}
			intermediate.insert(make_pair(i,loc));
		}
		
	}
/*Generating Symbol Table*/
	void generate_symtab(){
		for(int i=0;i<code.size();i++){
			string label=getlabel(code[i]);
			string opr=getopr(code[i]);
			string operand=getoperand(code[i]);
			if(label!="" && opr=="EQU"){
				if(operand=="*")
				symtab.insert(make_pair(label,intermediate[i]));
				else{
					string a="";
					string b="";
					char c='_';
					for(int i=0;i<operand.size();i++){
						if(operand[i]=='-')
							{c='-';continue;}
						if(c=='_')
							a.push_back(operand[i]);
						else
							b.push_back(operand[i]);

					}
					symtab.insert(make_pair(label,int_to_hex(hex_to_int(symtab[a])-hex_to_int(symtab[b]))));
				}
			}
			else if(label!=""){
				symtab.insert(make_pair(label,intermediate[i]));
			}
		}
	}
/*First pass of assembler*/
	void pass1(){
		generate_optab();
		assign_location();
		generate_symtab();
	}
/*Determing format and addressing mode of statement*/
	bool is_form1(string line){
		string opr=getopr(line);
		if(opr=="RSUB")
			return true;
		else
			return false;
	}

	bool is_form2(string line){
		string opr=getopr(line);
		if(opr=="CLEAR" || opr=="COMPR" || opr=="TIXR")
			return true;
		else
			return false;
	}

	bool is_form4(string line){
		if(line[10]=='+')
			return true;
		else
			return false;
	}

	bool is_form3(string line){
		if(is_form1(line) || is_form4(line) || is_form2(line))
			return false;
		else
			return true;
	}

	bool is_immediate(string line){
		if(line[19]=='#')
			return true;
		else
			return false;	 
	}

	bool  is_indirect(string line){
		if(line[19]=='@')
			return true;
		else
			return false;
	}
/*Parsing opcode*/
	string opcode_hex(string op,int no){
		int temp=0;
		string ans="";
		for(int i=0;i<op.size();i++){
			temp*=16;
			if(isalpha(op[i])){
				temp+=(op[i]-'A'+10);
			}
			else{
				temp+=(op[i]-'0');
			}

		}
		temp+=no;
		int i=2;
		while(i--){
			ans.insert(ans.begin(),hexcode[temp%16]);
			temp/=16;
		}
		return ans;
	}
/*Hexadecimal to integer conversion*/
	int hex_to_int(string op){
		int temp=0;
		for(int i=0;i<op.size();i++){
			temp*=16;
			if(isalpha(op[i])){
				temp+=(op[i]-'A'+10);
			}
			else{
				temp+=(op[i]-'0');
			}

		}
		return temp;
	}
/*calculating displacement from program counter*/
	string displacement(int pcline,string opd){
		if(getopr(code[pcline])=="BASE")
			pcline++;
		string pc=intermediate[pcline];
		string addr=symtab[opd];
		int a=hex_to_int(pc);
		int b=hex_to_int(addr);
		b-=a;
		bool flag=0;
		if(abs(b)>=2048){
			pc=symtab["LENGTH"];
			a=hex_to_int(pc);
			b=hex_to_int(addr);
			b-=a;
			flag=1;
		}

		if(b<0)
		{
			b=65535+b+1;
		}
		string t=int_to_hex(b);
		if(flag)
			t.push_back('*');
		return t;
	}

/*Generating machine codes of statements*/
	string mcode(string line,int no){
		string ans="";
		string opr=getopr(line);
		if(opr!="START" && opr!="END"){
		string opd=getoperand(line);
		if(assembler_dirc.find(opr)!=assembler_dirc.end()){
			if(opr=="BYTE")
			{
				ans=getoperand(line);
				ans=ans.substr(2,ans.size()-3);
				if(ans=="EOF")
					ans="454F46";
				return ans;
			}
			else{
				return "______";
			}

		}
		else if(is_form1(line)){
			
			return opcode_hex(optab[opr],3)+"0000";
		}
		else if(is_form2(line)){
			if(opr=="CLEAR"){
				return (optab[opr]+optab[opd]+"0");
			}
			if(opr=="COMPR"){
				string token1="",token2="";
				token1.push_back(opd[0]);
				token2.push_back(opd[2]);
				return (optab[opr]+optab[token1]+optab[token2]);
			}
			if(opr=="TIXR"){
				return (optab[opr]+optab[opd]+"0");
			}
		}
		else if(is_form4(line)){
			if(is_immediate(line)){
				if(!isalpha(opd[0]))
					return opcode_hex(optab[opr],1)+"10"+int_to_hex(string_to_int(opd));
				else
					return opcode_hex(optab[opr],1)+"10"+symtab[opd];

			}
			else if(is_indirect(line)){
				return "______";
			}
			else{
				return opcode_hex(optab[opr],3)+"10"+symtab[opd];
			}
		}
		else{
			
			if(is_immediate(line)){
				if(isalpha(line[20])){
					return (opcode_hex(optab[opr],1)+"2"+(displacement(no+1,opd)).erase(0,1));
				}
				else{
					return (opcode_hex(optab[opr],1)+"0"+int_to_hex(string_to_int(getoperand(line))).erase(0,1));	
				}	
			}
			else if(is_indirect(line)){
				return (opcode_hex(optab[opr],2)+"2"+(displacement(no+1,opd)).erase(0,1));
			}	
			else{
				if(opd[opd.length()-2]==',' && opd[opd.length()-1]=='X')
				{
					opd.erase(opd.size()-2,2);
					return opcode_hex(optab[opr],3)+"C"+((displacement(no+1,opd)).erase(0,1));	
				}
				
				string x=opcode_hex(optab[opr],3)+"2"+((displacement(no+1,opd)).erase(0,1));
				if(x[x.size()-1]=='*'){
					x[2]='4';
				}
				return x;
			}

			}
		}
		else{
			return "______";
		}
	}
/*Second pass of assembler*/
	void pass2(){
		for(int i=0;i<code.size();i++){
			string mac_code=mcode(code[i],i);
			if(mac_code[mac_code.size()-1]=='*')
				mac_code.erase(mac_code.size()-1,1);
			machine_code.push_back(mac_code);


		}
	}

/*Makes program independant of location (multi programming)*/
	void make_relocatable(){
		for(int i=0;i<code.size();i++){
			if(is_form4(code[i]) && !is_immediate(code[i])){
				rel_record.push_back("00"+int_to_hex(hex_to_int(intermediate[i])+1));
				}
		}
	}
/*Generating output as a string*/
	void generate_output(){
		output="H^";
		output.append(getlabel(code[0])+"  ^");
		output.append("00"+int_to_hex(string_to_int(getoperand(code[0])))+"^");
		output.append("00"+int_to_hex(hex_to_int(intermediate[code.size()-1])-hex_to_int(intermediate[0]))+"\n");
		int i=1;
		int pos=output.size()+8;
		while(i<code.size()){
			if(machine_code[i]=="______")
					{
						i++;
						continue;
					}
			output.append("T^");
			output.append("00"+intermediate[i]);
			int j;
			int size=0;
			for(;i<machine_code.size() && size<=30;i++){
				if(machine_code[i]=="______")
					{
						i++;
						break;
					}
				size+=(machine_code[i].size()/2);
				if(size<=30)	
				output.append("^"+machine_code[i]);
				else{
					size-=(machine_code[i].size()/2);
					break;
				}
			}
			output.insert(pos,"^"+int_to_hex(size).erase(0,2));
			output.append("\n");
			pos=output.size()+8;
		}

		for(int i=0;i<rel_record.size();i++){
			output.append("M^"+rel_record[i]+"^05\n");
		}

		output.append("E^00"+intermediate[1]+"\n");

	}

/*Creating output file*/
	void create_executable(const char * target){
		output_stream.open(target);
		output_stream<<output;
	}

	public:
/*Constructor*/
	assembler(const char * loc,const char * target){
		path=loc;
		parse();
		create_mnemonics();
		create_directives();
		pass1();
		pass2();
		make_relocatable();
		generate_output();
		create_executable(target);
	}
/*Destructor*/	
	~assembler(){
		file.close();
		output_stream.close();
	}

};


/*main*/

int main(int argc,char *argv[]){

	if(argc!=3)
	{	cout<<"\terror:Unspecified parameters\n";
		cout<<"\tUsage:\n ./assembler input.txt output.txt\n";
	}
	else{
/*assemblers object*/		
		assembler assemble(argv[1],argv[2]);
		cout<<"\n-----output assembled succesfully------\n";
	}
	return 0;
}