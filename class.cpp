#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>
#include <cstdlib>
#include <sstream>
#include "Parserbase.h"
using namespace std;

extern int l_no;
class type {
	public:
		string name;
		type *t;
		int dim;
		virtual void print()
		{
			cout << name ;
			if(t != 0)
			{
				if(name=="array")
					cout <<" of (" <<dim<<", ";
				else if(name=="pointer")
					cout <<" to ";
				else
					cout <<" ";
				t->print();
				if(name=="array")
					cout <<")";
			}
		}
};

class array_type : public type
{
	public:
		array_type(type *t1, int val)
		{
			name = "array";
			t = t1;
			dim = val;
		}
};

class pointer_type : public type
{
	public:
		pointer_type(type *t1)
		{
			name = "pointer";
			t = t1;
		}
};	

class base_type : public type //float, int, void, struct
{
	public:
		base_type(string s)
		{
			name = s;
			t = 0;
		}
};
namespace
{
	vector<string> rdata,text;
	int label = 0, slabel = 0, store_reg = 0, flabel = 0;
	string name_func="",name_struct="";
	bool equal(type *t1, type *t2) //check, seriously!
	{
		if(t1->t == 0 || t2->t == 0)
		{
			if(t1->name=="void" || t2->name=="void")
				return true;
			return t1->name == t2->name;
		}
		else if(t1->name == "pointer")
		{
			if(t2->name == "pointer" || t2->name == "array")
			{
				if(!equal(t1->t,t2->t))
				{
					return 0;
				}	
			}
			else 
			{
				return 0;
			}
		}
		else if(t1->name == "array")
		{
			if(t2->name == "pointer" || t2->name == "array")
			{
				if(!equal(t1->t,t2->t))
				{
					return 0;
				}
			}	
			else 
			{
				return 0;
			}
		}
		else 
		{
			return t1->name == t2->name; //for structs
		}
		return 1;
	}

	bool eqArray(type *a1, type* a2)
	{
		type * t1= a1;
		type * t2= a2;
		if(!equal(a1,a2))return 0;
		while((t1->t)->t !=0 && (t2->t)->t !=0)
		{
			if(t1->dim != t2->dim)return 0;
			t1=t1->t;
			t2=t2->t;
		}
		return 1;
	}

	bool eqPointerArray(type *a1, type* a2)
	{
		type * t1= a1;
		type * t2= a2->t;
		if(!equal(a1,a2))return 0;
		while((t1->t)->t !=0 && t2->t !=0)
		{
			if(t1->dim != t2->dim)return 0;
			t1=t1->t;
			t2=t2->t;
		}
		return 1;
	}

	void err(int code)
	{
		if(name_func!="")
			cerr << "test.c: In function '"<<name_func<<"':\n";
		else
			cerr << "test.c: In struct '"<<name_struct<<"':\n";
		cerr << "test.c:"<<l_no<<": error: ";
		if(code == 1)cerr << "struct redeclaration";
		if(code == 2)cerr << "cannot declare variable of type void";
		if(code == 3)cerr << "function redefinition";
		if(code == 4)cerr << "function not found";
		if(code == 5)cerr << "Incompatible types";
		if(code == 6)cerr << "Invalid operands";
		if(code == 7)cerr << "lvalue required as left operand of assignment";
		if(code == 8)cerr << "lvalue required as unary '&' operand";
		if(code == 9)cerr << "lvalue required as increment operand";
		if(code == 10)cerr << "Subscripted value is neither array nor pointer nor vector";
		if(code == 11)cerr << "too few arguments to function ‘printf’";
		if(code == 12)cerr << "Symbol not a pointer";
		if(code == 13)cerr << "Assignment to array not allowed";
		if(code == 14)cerr << "Indexing into an element which is not an array";
		if(code == 15)cerr << "Array index should be a non-negative integer";
		if(code == 17)cerr << "Incompatible array types";
		cerr<<endl;	
		exit(0);
	}
	void err(int code, string s)
	{
		if(name_func!="")
			cerr << "test.c: In function '"<<name_func<<"':\n";
		else
			cerr << "test.c: In struct '"<<name_struct<<"':\n";
		cerr << "test.c:"<<l_no<<": error: ";
		if(code == 1)cerr << "Undefined reference to "<<s;
		if(code == 2)cerr << "Invalid operand to "<<s; 
		if(code == 3)cerr << "struct '"<<s<<"' not found";
		if(code == 4)cerr << " called object ‘"<<s<<"’ is not a function or function pointer";
		if(code == 7)cerr <<"Redeclaration of variable '"<<s<<"'";
		if(code == 8)cerr <<"Incorrect arguments for function '"<<s<<"'";
		if(code == 9)cerr << "Using variable '"<<s<<"' without declaration";
		if(code == 10)cerr << "Size of array '"<<s<<"' has non-integer type";
		cerr<<endl;	
		exit(0);
	}
	void err(int code, string s, string s2)
	{
		if(name_func!="")
			cerr << "test.c: In function '"<<name_func<<"':\n";
		else
			cerr << "test.c: In struct '"<<name_struct<<"':\n";
		cerr << "test.c:"<<l_no<<": error: ";
		if(code == 1)cerr << "Symbol '"<<s<<"' not a member of struct '"<<s2<<"'";
		cerr<<endl;	
		exit(0);
	}
	void warning(type *n1, type * n2)
	{
		if(equal(n1, n2))return;
		cerr << "test.c: In function '"<<name_func<<"':\n";
		cerr << "test.c:"<<l_no<<": warning: ";
		cerr << "Assignment from incompatible type "<<n2->name<<" to "<<n1->name;
		cerr<<endl;	
	}
	void warning(int i)
	{
		if(i==1)
			cerr << "test.c: In function '"<<name_func<<"':\n"<< "test.c:"<<l_no<<": warning: "<<"function returns address of local variable."<<endl;
	}
	void generate_label()
	{
		label++;
		ostringstream ss;
		ss << label;
		string sss(ss.str());
		string content = sss+":";
		text.push_back(content);
	}
	void store_register(string s)
	{
		string content = "\taddi $sp $sp -4";
		text.push_back(content);
		store_reg++;
		if(s[0] == 't' || s == "fp" || s == "a0")
		{
			content = "\tsw $"+s+" 0($sp)";
			text.push_back(content);
		}
		else
		{
			content = "\tmfc1 $t";
			content += s[1];
			content += " $" + s;
			text.push_back(content);
			content = "\tsw $t";
			content += s[1];
			content += " 0($sp)";
			text.push_back(content);
		}
	}
	void store_word(string s, int x, string convert)
	{
		ostringstream ss;
		ss << x;
		string sss(ss.str());
		if(s[0] == 't' || s == "fp" || s == "v0")
		{
			string content;
			if(convert == "TO-FLOAT")
			{
				content = "\tmtc1 $";
				content += s;
				content += " $f";
				content += s[1];
				text.push_back(content);

				content = "\tcvt.s.w $f";
				content += s[1];
				content += " $f";
				content += s[1];
				text.push_back(content);

				content = "\tmfc1 $";
				content += s;
				content += " $f";
				content += s[1];
				text.push_back(content);
			}
			content = "\tsw $"+s;
			content += " "+sss;
			content += "($fp)";
			text.push_back(content);
		}
		else
		{
			string content;
			if(convert == "TO-INT")
			{
				content = "\tcvt.w.s $";
				content += s;
				content += " $";
				content += s;
				text.push_back(content);
			}
			content = "\tmfc1 $t";
			content += s[1];
			content += " $"+s;
			text.push_back(content);
			content = "\tsw $t";
			content += s[1];
			content += " "+sss;
			content += "($fp)";
			text.push_back(content);
		}
	}
	void store_struct(string s, int x, int wid, int off)
	{
		for (int i = 0; i < wid; i+=4)
		{
			ostringstream ss;
			ss << (x + i);
			string sss(ss.str());

			if(i != 0)
			{
				ostringstream ss;
				ss << (off + i);
				string sss(ss.str());

				string content = "\tlw $"+s;
				content += " "+sss;
				content += "($fp)";
				text.push_back(content);
			}

			string content = "\tsw $"+s;
			content += " "+sss;
			content += "($fp)";
			text.push_back(content);
		}
	}
}
class symbols
{
	public :
		string name;
		int width;
		type* t;
		int offset;
		bool param;
		symbols(string n, int w, int o, type* t1, bool p)
		{
			name = n;
			width = w;
			offset = o;
			t = t1;
			param = p;
		}
};

class symTab
{
	public:
		vector <symbols*> table;

		void put(string n, int w, int o, type* t, bool p)
		{
			symbols * x = new symbols(n, w, o, t, p);
			table.push_back(x);
		}
		int total_width()
		{
			int sum = 0;
			for(int i = 0; i <table.size(); i++)
				sum+=table[i]->width;
			return sum;
		}
		void print()
		{
			for (int i = 0; i < table.size(); ++i)
			{
				cout <<"\tname: "<<table[i]->name<<"; type: ";
				(table[i]->t)->print();
				cout <<"; width: "<<table[i]->width<<"; offset: "<<table[i]->offset<<"; is_param: "<<table[i]->param<<endl;
			}
		}
		bool InScope(string s)
		{
			for (int i = 0; i < table.size(); ++i)
			{
				if(table[i]->name==s)
				{
					return true;
				}
			}
			return false;
		}
		type * getType(string s)
		{
			for(int i = 0; i <table.size(); i++)
				if(table[i]->name == s)
					return table[i]->t;
		}
		int offset(string s)
		{
			for (int i = 0; i < table.size(); ++i)
			{
				if(table[i]->name==s)
				{
					return table[i]->offset;
				}
			}
		}
};

class globalSymbol
{
	public :
		int width;
		string name;
		type* t;
		int offset;
		symTab* table;
		vector<type *> params;
		globalSymbol(string n, int w, int o, type* t1, symTab* tab, vector<type*> v)
		{
			name = n;
			width = w;
			offset = o;
			t = t1;
			table = tab;
			params = v;
		}
		globalSymbol(string n, int w, int o, type* t1, symTab* tab)
		{
			name = n;
			width = w;
			offset = o;
			t = t1;
			table = tab;
		}
};

class globalSymTab
{
	public:
		vector <globalSymbol*> table;
		void put(string n, int w, int o, type* t, symTab* tab, vector<type*> v)
		{
			globalSymbol * x = new globalSymbol(n, w, o, t, tab, v);
			table.push_back(x);
		}
		void put(string n, int w, int o, type* t, symTab* tab)
		{
			globalSymbol * x = new globalSymbol(n, w, o, t, tab);
			table.push_back(x);
		}
		
		void printLast()
		{
			cout <<endl;
			int i = table.size()-1;
			
			cout <<"name: "<<table[i]->name<<"; return type: ";
			(table[i]->t)->print();
			cout <<"; width: "<<table[i]->width<<"; offset: "<<table[i]->offset;
			if((table[i]->t)->name!="STRUCT")
			{
				cout << "; parameters :";
				for(int j = 0; j <table[i]->params.size(); j++)
				{
					cout << table[i]->params[j]->name << " ";
				}
			}
			cout <<"; symbolTable: \n";
			cout <<string(100,'-')<<endl;

			(table[i]->table)->print();
			cout <<string(100,'_')<<endl;
			
		}
		int struct_size(string s)
		{
			for (int i = 0; i < table.size(); ++i)
			{
				if(table[i]->name==s)
					return (table[i]->table)->total_width();
			}
		}
		
		symTab * symbStruct(string n)
		{
			for(int i = 0; i <table.size(); i++)
			{
				if(table[i]->name == n && (table[i]->t)->name == "STRUCT")
					return table[i]->table;
			}
			cerr<<"err\n";
			err(1,n);
			return 0;
		}
		symTab * symbFunc(string n)
		{
			for(int i = 0; i <table.size(); i++)
			{
				if(table[i]->name == n && (table[i]->t)->name != "STRUCT")
					return table[i]->table;
			}
			err(1,n);
			return 0;
		}
		int findFunc(string n, vector <type*> para_type)
		{
			bool b = 0;
			int ret = 0;
			for(int i = 0; i <table.size(); i++)
			{
				if(table[i]->name == n && (table[i])->params.size() == para_type.size())
				{
					b =1;
					ret = 2;
					for(int j = 0; j <para_type.size(); j++)
					{
						string n1 = para_type[j]->name;
						string n2 = table[i]->params[j]->name;
						if(n1 == "pointer")
						{
							if(n2 == "int" || n2 == "pointer")
							{
								warning(para_type[j],table[i]->params[j]);
							}
							else if( n2 == "array")
							{
								if(!eqPointerArray(table[i]->params[j],para_type[j]))err(17);
							}
							else
								err(8,n);
						}
						else if(n1 == "array")
						{
							if(n2 == "int")
							{
								warning(para_type[j],table[i]->params[j]);
							}
							else if( n2 == "pointer")
							{
								if(!eqPointerArray(para_type[j], table[i]->params[j]))err(17);
							}
							else if( n2 == "array")
							{
								if(!eqArray(para_type[j], table[i]->params[j]))err(17);
							}
							else
								err(8,n);
						}
						else if(n1 == "int")
						{		
							if(n2 == "int" || n2 == "float" || n2 == "array"|| n2 == "pointer")
								warning(para_type[j],table[i]->params[j]);
							else
								err(8,n);
						}
						else if(n1 == "float")
						{	
							if(n2 == "int" || n2 == "float")
								warning(para_type[j],table[i]->params[j]);
							else
								err(8,n);
						}
						else if(n1 != n2)
							err(8,n);
					}
					if(b)
						return 1;
				}
				else if(table[i]->name == n && (table[i])->params.size() == 0)
				{
					return 1;
				}
			}
			return ret;
		}

		bool findFunc(string n)
		{
			for(int i = 0; i <table.size(); i++)
			{
				if(table[i]->name == n && (table[i]->t)->name != "STRUCT")return 1;
			}
			return 0;
		}
		bool findStruct(string n)
		{
			bool b = 0;
			for(int i = 0; i <table.size(); i++)
			{
				if(table[i]->name == n && (table[i]->t)->name=="STRUCT")
				{
					b = 1;
				}
			}
			return b;
		}
		type * findtype(string s)
		{
			for(int i = 0; i <table.size(); i++)
				if(table[i]->name == s)
					return table[i]->t;
			cerr<<"err\n";
			err(1,s);
		}
		int offsetStruct(string s,string n)
		{
			for(int i = 0; i <table.size(); i++)
				if(table[i]->name == s && (table[i]->t)->name=="STRUCT")
					return (table[i]->table)->offset(n);
		}
};

namespace
{
	globalSymTab *top = new globalSymTab();
	symTab *top_local = new symTab();
	string name;
	type* type1;
	type* old_type;
	type* ret;
	vector <type*> params;
	int old_width, width,offset,val,ptr=0,linked=0;
	
}

class abstract_astnode
{
	public:
		type *t;
		bool lvalue;
		virtual int print (int ident) = 0;
		// virtual int value () = 0;
		virtual string generate_code() = 0;
		// virtual basic_types getType() = 0;
		// virtual bool checkTypeofAST() = 0;
	// 	protected:
	// 	// virtual void setType(basic_types) = 0;
	// private:
		// typeExp astnode_type;
};

class stmt_astnode : public abstract_astnode
{
	public:
		string stmt_name;
		virtual int print(int ident)=0;
		virtual void validate(type *t){}
		virtual string generate_code() =0;
		virtual int offset(){}

};

class exp_astnode : public stmt_astnode
{
	public:
		int val;
		string exp_name;
		virtual int print(int ident) = 0;
		virtual void validate(){};
		virtual string generate_code() = 0;
		virtual int offset(){}
};

class unary_astnode : public exp_astnode
{
	private:
		exp_astnode * left;
	public:
		unary_astnode(string s, exp_astnode *l)
		{
			exp_name = s;
			if(s=="*")
			{
				lvalue = true;
				if(l->t == 0 || (l->t)->name!="pointer")
					err(2, "*");
				else
					t = (l->t)->t; 
			}
			else if(s=="&")
			{
				lvalue = false;
				if(!l->lvalue)
					err(8);
				t = new pointer_type(l->t);
			}
			else
			{
				lvalue = false;
				if(s=="PP" && !(l->lvalue))
					err(9);
				t = l->t;
			}
			left = l;

		}
		virtual int print(int ident)
		{			
			cout << "("<<exp_name << ' ';
			int x = left->print(ident + exp_name.size()+2);
			cout <<")"; 
			return x+3+exp_name.size();
		}	

		//t0- result, operands t1,t2
		virtual string generate_code()
		{
			left->generate_code();
			if(exp_name=="PP")
			{
				string content = "\taddi $t0 $t0 1";
				text.push_back(content);
			}
			if(exp_name == "!")					//fix this
			{
				string content = "\tnot $t0 $t0";
				text.push_back(content);
			}

			if(exp_name == "*")
			{
				string content = "\tlw $t0 0($t0)";
				text.push_back(content);
			}
			if(exp_name == "&")
			{
				int x = left->offset();
				string content = "\taddiu $t0 $fp "+x;
				text.push_back(content);
			}
			return "t0";
		}	
};

class binary_astnode : public exp_astnode
{
	private:
		exp_astnode * left, *right;
		int cast;
	public:
		binary_astnode(string s, exp_astnode *l, exp_astnode* r)
		{
			lvalue = false;
			exp_name = s;
			left = l;
			right = r;
			string n1 = (l->t)->name;
			string n2 = (r->t)->name;
			if(s!="AND_OP" && s!="OR_OP" && s!="EQ_OP" || s!="NE_OP")
			{
				if((n1=="int" || n1=="float") && (n2=="int" || n2=="float"))
				{
					if(n1!=n2)
					{
						if(n1=="int")
							cast=1;
						else
							cast=2;
						t = new base_type("float");
					}
					else
					{
						t = l->t;
						cast=0;
					}
				}
				else if(n1 == "int" && (n2 == "pointer" || n2 =="array"))
				{
					t = r->t;
				}
				else if(n2 == "int" && (n1 == "pointer" || n1 =="array"))
				{
					t = l->t;
				}
				else 
				{
					cerr << "Error: invalid operands for "<<s<<" at line number "<<l_no<<endl;
					exit(0);							//CHECK in future for correctness!!!
				}
			}
			else
				t = new base_type("int");
		}
		virtual int print(int ident)
		{
			cout << "("<<exp_name;
			//t->print();
			int x=0,y=0;
			if(cast==1)
			{
				cout <<"-FLOAT ( TO-FLOAT ";
				x = left->print(ident+ exp_name.size()+2+11+6);
				cout <<")";
				cout <<' ';
				y = right->print(ident+ exp_name.size()+2+11+x+1+1+6);
				cout <<")"; 
				return x+y+exp_name.size()+4+12+6;
			}
			else if(cast==2)	
			{
				cout <<"-FLOAT ";
				x = left->print(ident+ exp_name.size()+2+6);
				cout <<' ';
				cout <<"( TO-FLOAT ";
				y = right->print(ident+ exp_name.size()+2+11+x+1+6);
				cout <<")";
				cout <<")"; 
				return x+y+exp_name.size()+4+12+6;
			}
			else
			{
				if(t->name=="int")
				{
					cout <<"-INT ";
					x = left->print(ident+ exp_name.size()+2+4);
					cout <<' ';
					y = right->print(ident+ exp_name.size()+2+x+1+4);
					cout <<")"; 
					return x+y+exp_name.size()+4+4;
				}
				else if(t->name=="float")
				{
					cout <<"-FLOAT ";
					x = left->print(ident+ exp_name.size()+2+6);
					cout <<' ';
					y = right->print(ident+ exp_name.size()+2+x+1+6);
					cout <<")"; 
					return x+y+exp_name.size()+4+6;
				}
			}	
		}
		virtual string generate_code()
		{
			string s = left->generate_code();
			store_register(s);
			string s1 = right->generate_code();
			string conv="";
			string content = "\tlw $t1 0($sp)";
			text.push_back(content);
			if(s[0] == 'f')
			{
				content = "\tmtc1 $t1 $f1";
				text.push_back(content);
			}
			content = "\taddi $sp $sp 4";
			text.push_back(content);
			store_reg--;
			if(s[0] == 'f' && s1[0] == 't')
			{
				content = "\tmtc1 $t0 $f0";
				text.push_back(content);
				content = "\tcvt.s.w $f0 $f0";
				text.push_back(content);
				s1="f0";
				conv=".s";
			}
			else if(s1[0] == 'f' && s[0] == 't')
			{
				content = "\tmtc1 $t1 $f1";
				text.push_back(content);
				content = "\tcvt.s.w $f1 $f1";
				text.push_back(content);
				s="f1";
				conv=".s";
			}
			else if(s[0] == 'f' && s1[0] == 'f')
				conv=".s";
			if(exp_name == "AND_OP")
			{
				content = "\tand $t0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			else if(exp_name == "OR_OP")
			{
				content = "\tor $t0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			else if(exp_name == "NE_OP")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.ne.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tbne $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "EQ_OP")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.eq.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tbeq $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "LT")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.lt.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tblt $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "GT")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.gt.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tbgt $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "LE_OP")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.le.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tble $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "GE_OP")
			{
				int label1 = label +1;
				ostringstream ss;
				ss << label1;
				string sss(ss.str());
				if(s[0] == 'f')
				{
					content = "\tc.ge.s $f1 $f0";
					text.push_back(content);
					content += "\tbc1t "+sss;
					text.push_back(content);
				}
				else
				{
					content = "\tbge $t1 $t0 "+ sss;
					text.push_back(content);
				}
				content = "\taddi $t0 0 0";
				text.push_back(content);
				generate_label();
				content = "\taddi $t0 0 1";
				text.push_back(content);
			}
			else if(exp_name == "PLUS")
			{
				content = "\tadd"+conv;
				content += " $";
				content += s[0];
				content += "0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			else if(exp_name == "MINUS")
			{
				content = "\tsub"+conv;
				content += " $";
				content += s[0];
				content += "0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			else if(exp_name == "Multiply")
			{
				content = "\tmul"+conv;
				content += " $";
				content += s[0];
				content += "0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			else if(exp_name == "Divide")
			{
				content = "\tdiv"+conv;
				content += " $";
				content += s[0];
				content += "0 $";
				content += s1[0];
				content += "1 $";
				content += s[0];
				content += "0";
				text.push_back(content);
			}
			return s1;
		}
};

class float_astnode : public exp_astnode
{
	private:
		float val;
	public:
		float_astnode(float s)
		{
			lvalue = false;
			val = s; 
			exp_name = "floatconst";
			t= new base_type("float");
		}
		virtual int print(int ident)
		{
			cout << "(FloatConst "<<val<<")"; 
			string s = to_string(val);
			return 13 + s.size();
		}
		virtual string generate_code()
		{
			flabel++;
			ostringstream ss;
			ss << flabel;
			string sss(ss.str());

			ostringstream ss1;
			ss1 << val;
			string sss1(ss1.str());

			string content = "F" + sss;
			content += ":";
			rdata.push_back(content);
			content = "\t.float " + sss1;
			rdata.push_back(content);
			content = "\tl.s $f1 F"+ sss;
			text.push_back(content);
			return "f1";
		}
};

class int_astnode : public exp_astnode
{
	public:
		int_astnode(int s)
		{
			lvalue = false;
			val = s; 
			exp_name = "intconst";
			t = new base_type("int");
		}
		virtual int print(int ident)
		{
			cout << "(IntConst "<<val<<")"; 
			string s = to_string(val);
			return s.size() + 11;
		}
		virtual string generate_code()
		{
			ostringstream ss;
			ss << val;
			string sss(ss.str());
			string content = "\tli $t0 "+sss;
			text.push_back(content);
			return "t0"; 
		}
};

class string_astnode : public exp_astnode
{
	private:
		string val;
	public:
		string_astnode(string s)
		{
			lvalue = false;
			val = s;
			exp_name = "stringconst";
			t = new base_type("string");
		}
		virtual int print(int ident)
		{
			cout << "(StringConst "<<val<<")"; 
			return val.size() + 14;
		}
		virtual string generate_code()
		{
			slabel++;
			ostringstream ss;
			ss << slabel;
			string sss(ss.str());

			string content = "S" + sss;
			content += ":";
			rdata.push_back(content);
			content = "\t.ascii " + val;
			rdata.push_back(content);
			content = "\tla $a0 S" + sss;
			text.push_back(content);
			return "a0";
		}
};

class func_astnode : public exp_astnode  					//CHECK in future!!!
{
	private:
		string val; //identifier
		vector<exp_astnode*> list;
		vector<string> types;
	public:
		func_astnode(string s)
		{
			lvalue = false;
			val = s;
			if (val == "printf")
			{
				err(11);
			}
			else
				t = top->findtype(val);
		}
		
		func_astnode(string s, vector <exp_astnode*> a)
		{
			lvalue = false;
			val = s;
			list = a;
			if (val == "printf")
			{
				t = new base_type("syscall");
			}
			else
			{
				symTab * curr = top->symbFunc(s);
				if(curr->table.size()!=0)
				{
					for (int i = 0; i < list.size(); ++i)
					{
						if((list[i]->t)->name=="int" && ((curr->table[i])->t)->name=="float")
							types.push_back("TO-FLOAT");
						else if((list[i]->t)->name=="float" && ((curr->table[i])->t)->name=="int")
							types.push_back("TO-INT");
						else
							types.push_back("");
					}
				}
				t = top->findtype(val);
			}
		}
		virtual int print(int ident)
		{
			if (val != "printf")
			{
				int x = 0;
				cout << "(" <<val <<' ';
				if(!list.empty())
				{
					int x = 0, y = 0;
					if(types[0]!="")
					{
						cout <<"("<<types[0]<<" ";
						list[0]->print(ident+2+2+types[0].size()+val.size());
						cout <<")";
					}
					else	
						list[0]->print(ident+2+val.size());
					cout <<"\n";
					cout << string(ident+2+val.size(), ' ');
					for(int i = 1; i <list.size(); i++)
					{
						if(types[i]!="")
						{
							cout <<"("<<types[i]<<" ";
							y =list[i]->print(ident+2+2+types[i].size()+val.size());
							cout <<")";
						}
						else
							y =list[i]->print(ident+2+val.size());
						x = max(x,y);
						cout <<"\n";
						cout << string(ident+2+val.size(), ' ');
					}
				}
				cout <<")"; 
				return x + val.size() + 3;
			}
			else
				return val.size();
		}
		
		void validate()
		{
			if (val != "printf")
			{
				vector <type*> vec;
				for(int i = 0; i <list.size(); i++)
				{
					vec.push_back(list[i]->t);
				}
				if(top_local->InScope(val))
					err(4,val);
				int k = top->findFunc(val, vec);
				if(k == 0)err(8,val);	
				if(k == 2)err(8,val);
			}
		}
		virtual string generate_code()
		{
			if (val != "printf")
			{
				for(int i = 0; i <list.size(); i++)
				{
					string s = list[i]->generate_code();
					store_register(s);
				}	
				store_register("fp");
				// string content = "\tmove $fp $sp";
				// text.push_back(content);
				string content = "\tjal "+ val ;
				text.push_back(content);
			}
			else
			{
				for(int i = 0; i <list.size(); i++)
				{
					string s = list[i]->generate_code();
					if((list[i]->t)->name == "string")
					{
						string content = "\tli $v0 4";
						text.push_back(content);
					}
					else
					{
						if(s[0] == 't')
						{
							string content = "\tli $v0 1";
							text.push_back(content);
							content = "\tmove $a0 $"+s;
							text.push_back(content);
						}
						else if(s[0] == 'f')
						{
							string content = "\tli $v0 2";
							text.push_back(content);
							content = "\tmov.s $f12 $"+s;
							text.push_back(content);
						}
					}
					string content = "\tsyscall";
					text.push_back(content);
				}
			}
			return "v0";
		}
};		

class ass_astnode : public exp_astnode
{
	private:
		exp_astnode * left, *right;
		string convert;
	public:
		ass_astnode(exp_astnode *l, exp_astnode *r)
		{
			lvalue = false;
			if(!(l->lvalue))
				err(7);
			exp_name = "Ass";
			left = l;
			right = r;
			convert ="";

			string n1 = (l->t)->name;
			string n2 = (r->t)->name;
			if(n1 == "array")
			{
				err(13);
			}
			else if(n1 == "pointer")
			{				
				if(n2 == "int" || n2 == "array" || n2 == "pointer")
				{
					warning(l->t,r->t);
				}
				else
					err(5);
			}
			else if(n1 == "int")
			{
				if(n2 == "float")
					convert="TO-INT";
				if(n2 == "int" || n2 == "float" || n2 == "array"|| n2 == "pointer")
					warning(l->t,r->t);
				else
					err(5);
			}
			else if(n1 == "float")
			{			
				if(n2 == "int")
					convert="TO-FLOAT";		
				if(n2 == "int" || n2 == "float")
					warning(l->t,r->t);
				else
					err(5);
			}
			else if(n1 != n2)
				err(5);
			t = l->t;
		}
		virtual int print(int ident)
		{
			cout << "("<<exp_name << ' ';
			int x = left->print(ident+2+exp_name.size());
			cout <<' ';
			int y;
			if(convert!="")
			{
				cout <<"("<<convert<<" ";
				y = right->print(ident + 3 + 2 + convert.size() + exp_name.size()+ x);
				cout <<")";
				cout <<")"; 
				return exp_name.size()+x+y+4+3+convert.size();
			}
			y = right->print(ident + 3 + exp_name.size()+ x);
			cout <<")";
			cout <<")"; 
			return exp_name.size()+x+y+4;
		}
		virtual string generate_code()
		{
			string s = right->generate_code();
			int x = left->offset();
			if((right->t)->name == "Pointer" || (right->t)->name == "int" || (right->t)->name == "float" || (right->t)->name == "array")
				store_word(s, x, convert);
			else
			{
				int wid = top->struct_size((right->t)->name);
				int off = right->offset();
				store_struct(s,x,off,wid);
			}
			return "";
		}
};

class if_astnode : public stmt_astnode
{
	private:
		exp_astnode * left;
		stmt_astnode *middle, *right;
	public:
		if_astnode(exp_astnode *l, stmt_astnode * m, stmt_astnode *r)
		{
			lvalue = false;
			stmt_name = "If";
			left = l;
			right = r;
			middle = m;
		}
		virtual int print(int ident)
		{
			cout << "("<<stmt_name << ' ';
			int x = left->print(ident + 2 + stmt_name.size());
			cout <<"\n";
			cout <<string(ident + 4, ' ');
			int y = middle->print(ident + 4);
			cout <<"\n";
			cout <<string(ident + 4, ' ');
			int z = right->print(ident + 4);
			cout <<")"; 
			return max(max(y,z), x + 4);
		}
		virtual string generate_code()
		{
			int label1 = label +1;
			ostringstream ss;
			ss << label1;
			string sss(ss.str());
			int label2 = label +2;
			ostringstream ss1;
			ss1 << label2;
			string sss1(ss1.str());
			string s = left->generate_code();
			string content = "\tbeq $"+s;
			content += " $0 "+ sss;
			text.push_back(content);
			middle->generate_code();
			content = "\tj "+ sss1;
			text.push_back(content);
			generate_label();
			right->generate_code();
			generate_label();
			return "";
		}
};

class empty_astnode : public stmt_astnode
{
	public:
		empty_astnode()
		{
			lvalue = false;
			stmt_name = "Empty";
		}

		virtual int print(int ident)
		{
			cout << "("<<stmt_name<<")";
			return 2+ stmt_name.size(); 
		}
		virtual string generate_code()
		{
			return "";
		}
};


class return_astnode : public stmt_astnode
{
	private:
		exp_astnode * left;
		string convert;
	public:
		return_astnode(exp_astnode *l)
		{
			lvalue = false;
			stmt_name = "Return";
			left = l;
			convert = "";
		}

		virtual int print(int ident)
		{
			cout << "("<<stmt_name << ' ';
			int x;
			if(convert!="")
			{
				cout <<"("<<convert<<" ";
				x = left->print(ident + 2 + 2 + convert.size() + stmt_name.size());
				cout <<"))"; 
				return x  + stmt_name.size() + 3 + 3 + convert.size();
			}
			x = left->print(ident + 2 + stmt_name.size());
			cout <<")"; 
			return x  + stmt_name.size() + 3;
		}
		void validate(type * ret)
		{
			//top_local->print();
			string n1 = ret->name;
			string n2 = (left->t)->name;
			if(n1 == "array")
			{
				err(13);
			}
			else if(n1 == "pointer")
			{
				if(n2 == "array")
					warning(1);
				if(n2 == "int" || n2 == "pointer" || n2 == "array")
				{
					warning(ret,left->t);
				}
				else
					err(5);
			}
			else if(n1 == "int")
			{		
				if(n2 == "float")
					convert="TO-INT";
				if(n2 == "int" || n2 == "float" || n2 == "array"|| n2 == "pointer")
					warning(ret,left->t);
				else
					err(5);
			}
			else if(n1 == "float")
			{	
				if(n2 == "int")
					convert="TO-FLOAT";	
				if(n2 == "int" || n2 == "float")
					warning(ret,left->t);
				else
					err(5);
			}
			else if(n1 != n2)err(5);
		}
		virtual string generate_code()
		{
			string s = left->generate_code();
			string content = "\tmove $v0 $" + s;
			text.push_back(content);
			ostringstream ss;
			ss << store_reg*4;
			string sss(ss.str());
			if(store_reg != 0)
			{
				content = "\taddi $sp $sp " + sss;
				text.push_back(content);	
			}
			store_reg = 0;
			content = "\tmove $sp $fp";
			text.push_back(content);
			content = "\tjr $ra";
			text.push_back(content);
			return "v0";
		}
};		

class for_astnode : public stmt_astnode
{
	private:
		exp_astnode * left, *middle, *right;
		stmt_astnode *stmt;
	public:
		for_astnode(exp_astnode *l, exp_astnode *m, exp_astnode *r, stmt_astnode *s)
		{
			lvalue = false;
			stmt_name = "For";
			left = l;
			middle = m;
			right = r;
			stmt = s;
		}

		virtual int print(int ident)
		{
			cout << "("<<stmt_name <<' ';
			int x = left->print(ident+5);
			cout <<"\n";
			cout <<string(ident + 5, ' ');
			int y = middle->print(ident + 5);
			cout <<"\n";
			cout <<string(ident + 5, ' ');
			int z = right->print(ident + 5);
			cout <<"\n";
			cout <<string(ident + 5, ' ');
			int w = stmt->print(ident + 5);
			cout <<")"; 
			return max(5+x, max(y,max(z,w+1)));
		}
		virtual string generate_code()
		{
			left->generate_code();
			generate_label();
			int label1 = label +1;
			ostringstream ss;
			ss << label1;
			string sss(ss.str());
			string s1 = middle->generate_code();
			string content = "\tbeq $"+s1;
			content += " $0"+ sss;
			text.push_back(content);
			stmt->generate_code();
			right->generate_code();
			content = "\tj "+sss;
			text.push_back(content);
			generate_label();
			return "";
		}
};

class while_astnode : public stmt_astnode
{
	private:
		exp_astnode * left;
		stmt_astnode * stmt;
	public:
		while_astnode(exp_astnode *l, stmt_astnode *s)
		{
			lvalue = false;
			stmt_name = "While";
			left = l;
			stmt = s;
		}

		virtual int print(int ident)
		{
			cout << "("<<stmt_name << ' ';
			int x = left->print(ident+7);
			cout <<"\n";
			cout <<string(ident + 7, ' ');
			int y = stmt->print(ident + 7);
			cout <<")";
			return max(7+x, y+1); 
		}	
		virtual string generate_code()
		{
			generate_label();
			int label1 = label +1;
			ostringstream ss;
			ss << label1;
			string sss(ss.str());
			string s = left->generate_code();
			string content = "\tbeq $"+s;
			content += " $0 "+ sss;
			text.push_back(content);
			stmt->generate_code();
			content = "\tj "+sss;
			text.push_back(content);
			generate_label();
			return "";
		}
};

class list_astnode : public stmt_astnode
{
	private:
		stmt_astnode * stmt;
		stmt_astnode * list;
	public:
		list_astnode(stmt_astnode *s)
		{
			lvalue = false;
			stmt = s;
			list = 0;
		}
		list_astnode(stmt_astnode *s, stmt_astnode *l)
		{
			lvalue = false;
			stmt = s;
			list = l;
		}
		virtual int print(int ident)
		{
			int x = 0, y= 0;
			if(list != 0){
				x = list->print(ident);
				cout <<"\n";
				cout <<string(ident, ' ');
				y = stmt->print(ident);
			}
			else {
			y = stmt->print(ident);}
			return max(x, y);
		}
		virtual string generate_code()
		{
			if(list!=0)
				list->generate_code();
			stmt->generate_code();
			return "";
		}
};

class block_astnode : public stmt_astnode
{
	private:
		stmt_astnode *block;
	public:
		block_astnode(stmt_astnode *l)
		{
			lvalue = false;
			stmt_name = "Block";
			block = l;
		}

		virtual int print(int ident)
		{
			cout << "(Block [";
			int x = block->print(ident + 8);
			cout <<"])";
			return 10 + x;
		}	
		virtual string generate_code()
		{
			block->generate_code();
			return "";
		}
};

class ref_astnode : public exp_astnode
{
	public:
		string ref_name;
		virtual int print(int ident)=0;
		virtual string generate_code() = 0;
};

class id_astnode : public ref_astnode
{
	public:
		string id_name;
		id_astnode(string name)
		{
			lvalue = true;
			ref_name = "Id";
			id_name = name;
			if(ptr!=1)
			{
				if(top_local->InScope(name)==0)
				{
					err(9,name);
				}
			}
			else
				ptr=0;
			for (int i = 0; i < (top_local->table).size(); ++i)
			{
				if((top_local->table[i])->name==id_name)
					t = (top_local->table[i])->t;
			}
		}
		int offset()
		{
			return top_local->offset(id_name);
		}
		virtual string generate_code()
		{
			int off = top_local->offset(id_name);
			ostringstream ss;
			ss << off;
			string sss(ss.str());

			string content = "\tlw $t0 ";
			content += sss + "($fp)";
			text.push_back(content);
			if(t->name == "float")
			{
				content = "\tmtc1 $t0 $f0";
				text.push_back(content);
				content = "\tcvt.s.w $f0 $f0";
				text.push_back(content);
				return "f0";
			}
			return "t0";
		}
		virtual int print(int ident)
		{
			cout << "("<<ref_name<<" \"" <<id_name<<"\")" ;
			return 5+ref_name.size()+id_name.size();
		}
};

class member_astnode : public ref_astnode
{
	private:
		exp_astnode *id;
		id_astnode *mem;
	public:
		member_astnode(exp_astnode *a,id_astnode *b)
		{
			lvalue = true;
			id=a;
			mem=b;
			type * tp = a->t;
			while(tp->t !=0)tp = tp->t;
			symTab* s = top->symbStruct(tp->name);
			if(!s->InScope(mem->id_name))err(1,mem->id_name,tp->name);
			t = s->getType(mem->id_name);
		}

		virtual int print(int ident)
		{
			cout << "(Member ";
			int x = id->print(ident);
			cout <<" ";
			int y = mem->print(ident);
			cout <<")";
			return 5+x+y+5;
		}
		int offset()
		{
			int off1 = id->offset();
			string s = (id->t)->name;
			int off2 = top->offsetStruct(s, mem->id_name);
			return off1 + off2;
		}
		virtual string generate_code()
		{
			int off = this->offset();
			ostringstream ss;
			ss << off;
			string sss(ss.str());
			string content = "\tlw $t0 "+sss;
			content += "($fp)";
			text.push_back(content);

			type * tp = id->t;
			while(tp->t !=0)tp = tp->t;
			symTab* s = top->symbStruct(tp->name);
			type *t1 = s->getType(mem->id_name);

			if(t1->name == "float")
			{
				content = "\tmtc1 $t0 $f0";
				text.push_back(content);
				return "f0";
			}
			return "t0";
		}
};

class arrow_astnode : public ref_astnode
{
	private:
		exp_astnode *id;
		id_astnode *mem;
	public:
		arrow_astnode(exp_astnode *a,id_astnode *b)
		{
			lvalue = true;
			id=a;
			mem=b;
			if((id->t)->name != "pointer")err(12); //Not a pointer
			if((id->t)->t == 0)err(12);
			type * tp = a->t;
			while(tp->t !=0)tp = tp->t;
			symTab* s = top->symbStruct(tp->name);
			if(!s->InScope(mem->id_name))err(1,mem->id_name,tp->name);
			t = s->getType(mem->id_name);
		}

		virtual int print(int ident)
		{
			cout << "(Arrow ";
			int x = id->print(ident);
			cout <<" ";
			int y = mem->print(ident);
			cout <<")";
			return 6+x+y+3;
		}
		// int offset()
		// {
		// 	int off1 = id->offset();
		// 	string s = (id->t)->name;
		// 	int off2 = top->offsetStruct(s, mem->id_name);
		// 	return off1 + off2;
		// }
		virtual string generate_code()
		{
			int off = id->offset();
			ostringstream ss;
			ss << off;
			string sss(ss.str());
			string content = "\tlw $t0 "+sss;
			content += "($fp)";
			text.push_back(content);
			string s = (id->t)->name;
			
			int off1 = top->offsetStruct(s, mem->id_name);
			ostringstream ss1;
			ss1 << off1;
			string sss1(ss1.str());
			content = "\tlw $t0 "+sss1;
			content += "($t0)";
			text.push_back(content);
			return "t0";
		}
};

class arrref_astnode : public ref_astnode
{
	private:
		exp_astnode *id;
		exp_astnode *params;
	public:
		arrref_astnode(exp_astnode *a,exp_astnode * b)
		{
			lvalue = true;
			id=a;
			params = b;
			if((id->t)->name != "array")err(14);
			t = (id->t)->t;
			if(t == 0)err(10);
		}

		virtual int print(int ident)
		{
			cout << "(Arrayref";
			int x = id->print(ident+1);
			cout <<"[";
			int y = params->print(ident+2+x);
			cout <<"])";
			return 4+x+y+8;
		}
		void validate()
		{
			if(params-> t == NULL)err(15);
			if((params->t)->name != "int")err(15);
		}
		virtual int offset()
		{
			int off = id->offset();
			return off+4*params->val;
		}
		virtual string generate_code()
		{
			int off = this->offset();
			ostringstream ss;
			ss << off;
			string sss(ss.str());
			string content = "\tlw $t0 "+sss;
			content += "($fp)";
			text.push_back(content);
			type *temp =id->t;
			while((temp->t)->t!=0)
			{
				temp = temp->t;
			}
			if((temp->t)->name=="float")
			{
				content = "\tmfc1 $t0 $f0";
				text.push_back(content);
				return "f0";
			}
			return "t0";
		}
};		

class ptr_astnode : public exp_astnode
{
	private:
		ref_astnode *id;
	public:
		ptr_astnode(ref_astnode *a)
		{
			lvalue = false;
			id=a;
			t = new pointer_type(a->t);
		}

		virtual int print(int ident)
		{
			cout << "(Pointer ";
			int x = id->print(ident);
			cout <<")";
			return x+4+6;
		}
};

class deref_astnode : public ref_astnode
{
	private:
		ref_astnode *id;
	public:
		deref_astnode(ref_astnode *a)
		{
			lvalue = true;
			id=a;
			t = (a->t)->t;
		}

		virtual int print(int ident)
		{
			cout << "(Deref ";
			int x = id->print(ident);
			cout <<")";
			return 4 + x + 4;
		}
};
