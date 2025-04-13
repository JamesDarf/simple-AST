#include <stdio.h>
#include "json_c.c"

int main() {

    // file open
    FILE *fp = fopen("ast.json", "r");
    if (!fp) {
        perror("파일 열기 실패");
        return 1;
    } //


    //Creation and assignment of a json_value
	//Dynamic allocation used
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

    char *json_str = (char *)malloc(length + 1);
    fread(json_str, 1, length, fp);
    json_str[length] = '\0';
    fclose(fp);

    // 3. JSON 객체 만들기
    json_value json = json_create(json_str);
    free(json_str);

    // json_print(json); // print json

    // 4. ext 배열 가져오기
    json_value ext = json_get(json, "ext");
    int ext_len = json_len(ext);

    int func_count = 0;

    // 5. 배열 순회하며 FuncDef만 추출
    for (int i = 0; i < ext_len; i++) {
        json_value item = json_get(ext, i);
        char *nodetype = json_get_string(item, "_nodetype");

        if (strcmp(nodetype, "FuncDef") == 0) {
            func_count++;


            // decl → name
            json_value decl = json_get(item, "decl");
            char *name = json_get_string(decl, "name");


            // decl → type → type → type → names[0]
            json_value t1 = json_get(decl, "type");
            json_value t2 = json_get(t1, "type");
            json_value t3 = json_get(t2, "type");
            json_value names = json_get(t3, "names");
            char *ret_type = json_get_string(names, 0);


            // 6. 출력
            printf("함수 이름 : %s\n", name);
            printf("리턴 타입 : %s\n\n", ret_type);

            json_value args = json_get(t1, "args");
            json_value params = json_get(args, "params");
            int param_len = json_len(params);



            printf("파라미터     : ");
            if (param_len == 0) {
                printf("(없음)\n");
            } else {
                for (int j = 0; j < param_len; j++) {
                    json_value param = json_get(params, j);
                    char *pname = json_get_string(json_get(param, "name"));  // 변수명


                    // 타입
                    json_value ptype1 = json_get(param, "type");
                    json_value ptype2 = json_get(ptype1, "type");
                    json_value pnames = json_get(ptype2, "names");
                    char *ptype = json_get_string(json_get(pnames, 0)); // 예: int, char 등


                    // 포인터 확인
                    json_value quals = json_get(ptype1, "quals");
                    int is_pointer = 0;
                    for (int q = 0; q < json_len(quals); q++) {
                        char *qval = json_get_string(json_get(quals, q));
                        if (strcmp(qval, "*") == 0) {
                            is_pointer = 1;
                            break;
                        }
                    }


                    // output
                    if (j != 0) printf(", ");
                    printf("%s%s %s", ptype, is_pointer ? "*" : "", pname);
                }
                printf("\n");
            }

            printf("\n-------------test------------\n");
        }
    }



    // create json object
    printf("함수 총 개수 : %d개\n", func_count);

    return 0;
}