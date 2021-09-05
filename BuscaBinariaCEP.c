#include <stdio.h>
#include <string.h>

typedef struct _Endereco Endereco;

struct _Endereco
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
};

int main(int argc, char **argv)
{
    FILE *f;
    Endereco e;
    int qt, c, achei;
    long tamanhoArquivo, tamanhoRegistro, totalRegistros, posicao, inicio, ultimo, meio;

    //if (argc != 2)
    //{
    //    fprintf(stderr, "USO: %s [CEP]", argv[0]);
    //    return 1;
    //}

    // contar os números de passos
    c = 0;
    achei = 0;

    // Acessar o tamanho total do arquivo (tamarq)
    f = fopen("cep_ordenado.dat", "r");
    fseek(f, 0, SEEK_END);
    tamanhoArquivo = ftell(f);
    rewind(f); // fseek(f,0,SEEK_SET);
    printf("(1) Tamanho do Arquivo  : %ld\n", tamanhoArquivo);

    // Saber o tamanho de um registro (tamreg)
    tamanhoRegistro = sizeof(Endereco);
    totalRegistros = tamanhoArquivo / tamanhoRegistro;
    printf("(2) Tamanho de um Registro : %ld\n", tamanhoRegistro);
    printf("(3) Tamanho de Registros existentes (%ld/%ld) : %ld\n", tamanhoArquivo, tamanhoRegistro, totalRegistros);

    // atribui inicio
    inicio = 0;
    printf("(4) INICIO :\n");
    printf("Registro %d : ", ftell(f) * tamanhoRegistro);
    fread(&e, sizeof(Endereco), 1, f);
    printf(" %.72s / %.72s / %.72s / %.72s / %.2s / %.8s -> ", e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);
    printf("Posicao Atual : %ld\n", ftell(f)); // em bytes

    // atribui o ultimo (pela primeira vez)
    ultimo = totalRegistros - 1;

    // atribui o meio (pela primeira vez)
    meio = (inicio + ultimo) / 2;
    printf("(5) MEIO :\n");
    printf("Registro %ld : ", meio);
    fseek(f, meio * tamanhoRegistro, SEEK_SET);
    //printf("Posicao Atual : %ld\n", ftell(f)); // em bytes (imprime 104 milhoes bytes)
    fread(&e, tamanhoRegistro, 1, f);
    printf(" %.72s / %.72s / %.72s / %.72s / %.2s / %.8s -> ", e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);
    printf("Posicao Atual : %ld\n", ftell(f)); // em bytes

    // imprime o ultimo (primeiramente)
    printf("(6) ULTIMO :\n");
    printf("Registro %ld : ", ultimo);
    fseek(f, ultimo * tamanhoRegistro, SEEK_SET);
    fread(&e, sizeof(Endereco), 1, f);
    printf(" %.72s / %.72s / %.72s / %.72s / %.2s / %.8s -> ", e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);
    printf("Posicao Atual : %ld\n", ftell(f)); // em bytes

    // agora é que começa o programa ...
    rewind(f);

    while (inicio <= ultimo && achei == 0)
    {
        c++;
        meio = (inicio + ultimo) / 2;
        fseek(f, meio * tamanhoRegistro, SEEK_SET);
        fread(&e, tamanhoRegistro, 1, f);
        // compara com o cep pesquisado
        // e.cep[7] = '\0';
        // pode usar o strsstr
        if (strncmp(argv[1], e.cep, 8) == 0)
        {
            printf("(7) Achei o CEP informado no Registro %ld : ", meio);
            printf(" %.72s / %.72s / %.72s / %.72s / %.2s / %.8s -> ", e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);
            printf("Posicao Atual : %ld\n\n", ftell(f)); // em bytes
            achei = 1;
            break; // colocar o ponto, delimita realmente o numero de caracteres
        }
        else
        {
            if (strncmp(argv[1], e.cep, 8) > 0)
            {
                inicio = meio + 1;
            }
            else
            {
                ultimo = meio - 1;
            }
        }
    }
    if (achei == 0)
    {
        printf("(7) NAO ACHEI !\n\n");
    }
    printf("(8) Gastei o total de %ld passos !!!\n", c);
    // fechar o arquivo
    fclose(f);
    return 0;
}