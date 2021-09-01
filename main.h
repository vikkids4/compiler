typedef enum {
	typeCls, typeVar
} nodeEnum;

/* constants */
typedef struct {
	char *name; /* name of class */
} clsNodeType;

typedef struct {
	char *name; /* name of class */
    char *type;
    char *value;
} varNodeType;

typedef struct nodeTypeTag {
	nodeEnum type; /* type of node */

	union {
		clsNodeType cls; /* classes */
        varNodeType var; /* variables */
	};
} nodeType;

struct symbols{
    nodeType node;
};