
%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP GE_OP LE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN STRUCT IDENTIFIER INC_OP PTR_OP
%polymorphic STRING : std::string; EXPAST : exp_astnode*; STMAST : stmt_astnode*; REFAST : ref_astnode*; INT1 : int; FLOAT1 : float; LIST : std::vector<exp_astnode*>; 
%type<STRING> unary_operator INC_OP IDENTIFIER STRING_LITERAL
%type<INT1> INT_CONSTANT
%type<FLOAT1> FLOAT_CONSTANT
%type<EXPAST> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression constant_expression logical_or_expression postfix_expression
%type<STMAST> compound_statement statement selection_statement iteration_statement statement_list assignment_statement
%type<LIST> expression_list
%type<REFAST> declarator parameter_declaration

%%

M 
	: translation_unit
	{
		if(!top->findFunc("main"))
		{
			cerr << "test.c:"<<l_no<<": error: ";
			cerr<<"function 'main' not found"<<endl;
			ABORT();
		}
		cout<<"\t.rdata"<<endl;	 	
 		for (int i = 0; i < rdata.size(); ++i)
 		{
 			cout<<rdata[i]<<endl;
 		}
 		cout<<"\t.text"<<endl;	 	
 		for (int i = 0; i < text.size(); ++i)
 		{
 			cout<<text[i]<<endl;
 		}	
	}
	;
translation_unit 
        :  struct_specifier
        {
        	//top->printLast();
	 	}
	 	| function_definition 
	 	{
	 		//top->printLast();
	 	}
	 	| translation_unit function_definition 
	 	{	 		
	 		//top->printLast();
	 	}
        | translation_unit struct_specifier
        {    
	 		//top->printLast();
	 	}
        ;

struct_specifier 
        : STRUCT IDENTIFIER '{' 
        { 
        	name_struct = $2;
        	name_func = "";
        	top_local = new symTab();
        }
        declaration_list '}' ';'
        { 
        	if(top->findStruct($2))err(1);
			top->put($2,top_local->total_width(),0,new base_type("STRUCT"),top_local);
			name_struct="";
		}
        ;

function_definition
	: type_specifier {top_local = new symTab();ret = type1; offset = 0; params.clear();} fun_declarator 
	{
		if(!params.empty())
		{
			int n = top_local->table.size();
			int w = (top_local->table[n-1])->width;
			(top_local->table[n-1])->offset = -w;
			for(int i=(top_local->table).size()-2;i>=0;i--)
			{
				w+=(top_local->table[i])->width;
				(top_local->table[i])->offset = -w;
			}
		}
		offset = 0;
		if(!params.empty())
			top->put(name_func,top_local->total_width(),0,ret,top_local,params);
		else
			top->put(name_func,top_local->total_width(),0,ret,top_local);
		text.push_back(name_func+":");
	} 
	compound_statement 
	{}
	;

type_specifier                   // This is the information 
        : VOID 	                 // that gets associated with each identifier
        {
        	type1 = old_type = new base_type("void");
    	  	width = old_width = 0;
    	}
        | INT
        {
        	type1 = old_type = new base_type("int");
    	  	width = old_width = 4;
    	}   
		| FLOAT
		{			
			type1  = old_type = new base_type("float");
			width  = old_width = 4;
		} 
        | STRUCT IDENTIFIER 
        {
        	if(!(top->findStruct($2)) && $2!=name_struct)
        		err(3,$2);
        	if($2==name_struct)
        	{
        		linked=1;
        	}
        	type1 = old_type = new base_type($2);
        	width = old_width = top->struct_size($2);  	
        }
        ;

fun_declarator
	: IDENTIFIER '(' parameter_list ')' 
	{
		if(top->findFunc($1))err(3);
		name_func = $1;
	}
	| IDENTIFIER '(' ')' 
	{
		if(top->findFunc($1))err(3);	
		name_func = $1;
	}
    | '*' fun_declarator  //The * is associated with the 
    {    	
    	ret = new pointer_type(ret);
    }
	;                      //function name


parameter_list
	: parameter_declaration 
	{		
		top_local->put(name,width,offset,type1,1);
		offset+=width;
		params.push_back(type1);
		while(type1 !=0)
		{
			type1=type1->t;
		}
	}
	| parameter_list ',' parameter_declaration 
	{		
		top_local->put(name,width,offset,type1,1);
		offset+=width;
		params.push_back(type1);
		while(type1 !=0)
		{
			type1=type1->t;
		}
	}
	;

parameter_declaration
	: type_specifier declarator 
	;

declarator
	: IDENTIFIER 
	{
		if(linked==1)
		{
			err(3,name_struct);
		}	
		name = $1;
		if(top_local->InScope(name))
		{
			err(7,name);
		}
	}
	| declarator '[' primary_expression']' // check separately that it is a constant
	{		
		if(linked==1)
			err(3,name_struct);
		if(($3)->exp_name != "intconst")err(10,name);
		width*=($3)->val;
		type1 = new array_type(type1, ($3)->val);
	}
    | '*' 
    {
    	if(linked==1)
    	{
    		linked=2;
    	}
    }
    declarator 
    {
    	type1 = new pointer_type(type1);
    }
    ;

primary_expression 
        : IDENTIFIER
        {        	
        	$$ = new id_astnode($1);
        }
        | INT_CONSTANT
        {        	
        	$$ = new int_astnode($1);
        	val=$1;
        } 
        | FLOAT_CONSTANT
        {        	
        	$$ = new float_astnode($1);
        }
        | STRING_LITERAL
        {        	
        	$$ = new string_astnode($1);
        }
        | '(' expression ')'
        {        	
        	$$ = $2;
        } 
        ;

compound_statement
	: '{' '}' 
	{		
		$$ = new empty_astnode();
		//($$)->print(0);
	}
	| '{' statement_list '}' 
	{
		$$ = new block_astnode($2);
		//($$)->print(0);
		text.push_back("\tmove $fp $sp");
		($$)->generate_code();
	}
    | '{' declaration_list statement_list '}' 
    {
		$$ = new block_astnode($3);
		//($$)->print(0);
		text.push_back("\tmove $fp $sp");
		($$)->generate_code();
	}
	;

statement_list
	: statement		
	{		
		$$ = new list_astnode($1);
	}
    | statement_list statement	
    {    	
    	$$ = new list_astnode($2, $1);
    }
	;

statement
        : '{' statement_list '}'  
        {        	
        	$$ = new block_astnode($2); 
        }
        | selection_statement
		{			
			$$ = $1;
		} 	
        | iteration_statement
        {        	
        	$$ = $1;
        } 	
		| assignment_statement
		{			
			$$ = $1;
		}
        | RETURN expression ';'
        {       	
        	$$ = new return_astnode($2);
        	($$)->validate(ret);
        }	
        ;

assignment_statement
	: ';'
	{		
		$$ = new empty_astnode(); 
	} 								
	|  expression ';'
	{		
		$$ = $1;
	}  
	;

expression             //assignment expressions are right associative
        :  logical_or_expression
        {        	
        	$$ = $1;
        }
        |  unary_expression '=' expression
        {        	
        	$$ = new ass_astnode($1,$3);
        } 
        ;

logical_or_expression            // The usual hierarchy that starts here...
	: logical_and_expression
	{		
		$$ = $1;
	}
    | logical_or_expression OR_OP logical_and_expression
    {    	
    	$$ = new binary_astnode("OR_OP",$1,$3);
    }
	;
logical_and_expression
        : equality_expression
        {        	
        	$$ = $1;
        }
        | logical_and_expression AND_OP equality_expression
        {        	
        	$$ = new binary_astnode("AND_OP",$1,$3);
        } 
	;

equality_expression
	: relational_expression
	{		
		$$ = $1;
	} 
    | equality_expression EQ_OP relational_expression
    {    	
    	$$ = new binary_astnode("EQ_OP",$1,$3);
    } 	
	| equality_expression NE_OP relational_expression
	{		
		$$ = new binary_astnode("NE_OP",$1,$3);
	}
	;
relational_expression
	: additive_expression
	{		
		$$ = $1;
	}
    | relational_expression '<' additive_expression 
    {    	
    	$$ = new binary_astnode("LT",$1,$3);
    }
	| relational_expression '>' additive_expression
	{		
		$$ = new binary_astnode("GT",$1,$3);
	} 
	| relational_expression LE_OP additive_expression
	{		
		$$ = new binary_astnode("LE_OP",$1,$3);
	} 
    | relational_expression GE_OP additive_expression
    {    	
    	$$ = new binary_astnode("GE_OP",$1,$3);
    } 
	;

additive_expression 
	: multiplicative_expression
	{		
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression
	{		
		$$ = new binary_astnode("PLUS",$1,$3);
	} 
	| additive_expression '-' multiplicative_expression
	{		
		$$ = new binary_astnode("MINUS",$1,$3);
	} 
	;

multiplicative_expression
	: unary_expression
	{		
		$$ = $1;
	}
	| multiplicative_expression '*' unary_expression
	{		
		$$ = new binary_astnode("Multiply",$1,$3);
	} 
	| multiplicative_expression '/' unary_expression
	{		
		$$ = new binary_astnode("Divide",$1,$3);
	} 
	;
unary_expression
	: postfix_expression
	{		
		$$ = $1;
	}  				
	| unary_operator unary_expression
	{		
		$$ = new unary_astnode($1,$2);
	} 
	;

postfix_expression
	: primary_expression
	{		
		$$ = $1;
	}  				
    | IDENTIFIER '(' ')'
    {    	
    	$$ = new func_astnode($1);
    	($$)->validate();
    } 				
	| IDENTIFIER '(' expression_list ')'
	{
		$$ = new func_astnode($1, $3);
		($$)->validate();
	}
    | postfix_expression '[' expression ']'
    {
    	if(top_local->InScope($1->exp_name)){};
    	$$ = new arrref_astnode($1, $3); 
    	($$)->validate(); 
    }  
    | postfix_expression '.' IDENTIFIER
    {    	
    	ptr=1;
    	id_astnode *x = new id_astnode($3);
    	$$ = new member_astnode($1,x);
    }
    | postfix_expression PTR_OP IDENTIFIER
	{		
		ptr=1;
		id_astnode *x = new id_astnode($3);
   		$$ = new arrow_astnode($1,x);
    } 
	| postfix_expression INC_OP 	// ... and ends here
	{		
		$$ = new unary_astnode("PP", $1);
	}			
	;

expression_list
        : expression
        {        	
        	vector<exp_astnode*> v;
        	v.push_back($1);
        	$$= v;
        }
        | expression_list ',' expression
        {
        	($1).push_back($3);
        	$$ = $1;
        }
	;

unary_operator
        : '-'
        {        	
        	$$ = "-";
        }
		| '!'
		{			
			$$ = "!";
		}
        | '&'
        {        	
        	$$ = "&";
        }
        | '*' 	
        {        	
        	$$ = "*";
        }
	;

selection_statement
        : IF '(' expression ')' statement ELSE statement
        {        	
        	$$ = new if_astnode($3, $5, $7);
        } 
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{		
		$$ = new while_astnode($3, $5);
	}
	| FOR '(' expression ';' expression ';' expression ')' statement  //modified this production
	{		
		$$ = new for_astnode($3,$5, $7, $9);
	}
    ;

declaration_list
        : declaration  		
        {        	
        }			
        | declaration_list declaration
        {        	
        }
	;

declaration
	: type_specifier declarator_list';' 
	{		
	}
	;

declarator_list
	: declarator 
	{	
		if(type1->name=="void")	
			err(2);
		top_local->put(name,width,offset,type1,0);
		while(type1 !=0)
		{
			type1=type1->t;
		}
		offset+=width;
		width = old_width;
		type1 = old_type;
	}
	| declarator_list ',' declarator 
	{		
		linked=0;
		top_local->put(name,width,offset,type1,0);
		while(type1 !=0)
		{
			type1=type1->t;
		}
		offset+=width;
		width = old_width;
		type1 = old_type;
	}
 	;
