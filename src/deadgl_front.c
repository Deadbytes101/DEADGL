#include "deadgl.h"
#include <string.h>
int deadgl_render_main(int argc,char**argv);
int dgl_inspect_file(const char*path);
int main(int argc,char**argv){if(argc==3&&(strcmp(argv[1],"inspect")==0||strcmp(argv[1],"audit")==0)){return dgl_inspect_file(argv[2]);}return deadgl_render_main(argc,argv);}
