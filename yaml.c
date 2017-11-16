
#include <stdio.h>
#include <yaml.h>

typedef struct Conf {
  char* db_server;
  char* db_pass;
  char* db_user;
};

typedef struct MassConf {
  struct Conf* server1;
  struct Conf* server2;
};

struct MassConf* readConf(char*);

int main(int argc, char * argv[]){
  struct MassConf* cf = readConf("conf.yaml");
  printf("cf1: %s %s %s\n", cf->server1->db_server, cf->server1->db_pass, cf->server1->db_user);
  printf("cf2: %s %s %s\n", cf->server2->db_server, cf->server2->db_pass, cf->server2->db_user);
}
  
struct MassConf* readConf(char* filename) {
  FILE* fh = fopen(filename, "r");
  yaml_parser_t parser;
  yaml_token_t token;
  struct MassConf* conf = malloc(sizeof(struct MassConf));
  conf->server1 = malloc(sizeof(struct Conf));
  conf->server2 = malloc(sizeof(struct Conf));

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
    struct Conf* confp;
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
          datap = &confp->db_server;
        } else if (!strcmp(tk, "db_pass")) {
          datap = &confp->db_pass;
        } else if (!strcmp(tk, "db_user")) {
          datap = &confp->db_user;
        } else if (!strcmp(tk, "server1")) {
          confp = conf->server1;
        } else if (!strcmp(tk, "server2")) {
          confp = conf->server2;
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
