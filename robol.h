typedef enum { typeCon, typeId, typeOpr, typeFunction } nodeEnum;

struct symbol {
	char *name;
	int value;
	int tip;
	int declared;
	struct nodeTypeTag *op[1];
};

struct function {
	char *name;
	struct nodeTypeTag *op[2];
};

/* constants */
typedef struct {
    int value;                  /* value of constant */
	int s_type;					//type 0 is number, type 1 is direction
} conNodeType;

/* identifiers */
typedef struct {
	int i;                      /* subscript to sym array */
	int s_type;
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
	int s_type;					// type of node, broj or nasoka
    struct nodeTypeTag *op[1];	/* operands, extended at runtime */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
		struct function fun;
		
    };
} nodeType;

#define NHASH 9997
extern struct symbol symtab[NHASH];
//extern int symtab[NHASH];
