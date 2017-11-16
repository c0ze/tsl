
#include <stdio.h>
#include <yaml.h>

typedef struct Conf {
  char* db_server;
  char* db_pass;
  char* db_user;
};

struct Conf* readConf(char*);

int main(int argc, char * argv[]){
  struct Conf* cf = readConf("conf.yaml");
  printf("cf: %s %s %s", cf->db_server, cf->db_pass, cf->db_user);
}
  
struct Conf* readConf(char* filename) {
  FILE* fh = fopen(filename, "r");
  yaml_parser_t parser;
  yaml_token_t token;
  struct Conf* conf = malloc(sizeof(struct Conf));

  if (!yaml_parser_initialize(&parser))
    fputs("Failed to initialize parser!\n", stderr);
  if (fh == NULL)
    fputs("Failed to open file!\n", stderr);
  yaml_parser_set_input_file(&parser, fh);

  int state = 0;
  do {
    /* As this is an example, I'll just use:
     *  state = 0 = expect key
     *  state = 1 = expect value
     */

    char** datap;
    char* tk;

    yaml_parser_scan(&parser, &token);
    switch(token.type)
    {
    case YAML_KEY_TOKEN:
      state = 0;
      //      printf("got key: %d %s\n", state, token.data.scalar.value);
      break;
    case YAML_VALUE_TOKEN:
      state = 1;
      //printf("got value: %d %s\n", state, token.data.scalar.value);
      break;
    case YAML_SCALAR_TOKEN:
      //printf("got scalar: %d %s\n", state, token.data.scalar.value);
      //      break;
      tk = token.data.scalar.value;
      if (state == 0) {
        /* It's safe to not use strncmp as one string is a literal */
        if (!strcmp(tk, "db_server")) {
	  //	  printf("got db_server");
          datap = &conf->db_server;
        } else if (!strcmp(tk, "db_pass")) {
          datap = &conf->db_pass;
        } else if (!strcmp(tk, "db_user")) {
          datap = &conf->db_user;
        } else {
          printf("Unrecognised key: %s\n", tk);
        }
      } else {
        *datap = strdup(tk);
      }
      break;
    default: break;
    }
    if (token.type != YAML_STREAM_END_TOKEN)
      yaml_token_delete(&token);
  } while (token.type != YAML_STREAM_END_TOKEN);
  yaml_token_delete(&token);

  yaml_parser_delete(&parser);
  fclose(fh);
  return conf;
}
