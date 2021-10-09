#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Definindo o struct Endereco
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

int compara(const void *e1, const void *e2)
{
    return strncmp(((Endereco *)e1)->cep, ((Endereco *)e2)->cep, 8);
}

int main()
{
    // Definicao de todas as variaveis
    FILE *f, *g, *h;
    Endereco e, *n, ea, eb;
    int c, i, j, w, resposta;
    int base[4] = {2, 4, 8, 16};
    int arq_base[4];
    char nomeArquivo[100], nomeArquivo1[100], nomeArquivo2[100];
    long tamanhoArquivo, tamanhoRegistro, totalRegistros, tamanhoSubArquivos[2], posicao, qtd;

    ////////////////////////////////////////////////////////////////////////////////////
    // 1. Acessar o tamanho total do arquivo (tamanhoArquivo), e pegar dados necessarios
    ////////////////////////////////////////////////////////////////////////////////////
    f = fopen("cep.dat", "r");
    fseek(f, 0, SEEK_END);
    tamanhoArquivo = ftell(f);
    rewind(f); // = fseek(f,0,SEEK_SET);
    printf("(1) TAMANHO DO ARQUIVO  : %ld\n", tamanhoArquivo);

    // Saber o tamanho de um registro (tamanhoRegistro)
    tamanhoRegistro = sizeof(Endereco);
    totalRegistros = tamanhoArquivo / tamanhoRegistro;
    printf("(2) TAMANHO DE UM REGISTRO : %ld\n", tamanhoRegistro);
    printf("(3) TAMANHO DE REGISTROS EXISTENTES (%ld/%ld) : %ld\n", tamanhoArquivo, tamanhoRegistro, totalRegistros);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Perguntar quantos arquivos desejados (nas 4 bases possiveis), e os tamanhos respectivos de cada
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    
    printf("(4) NUMERO DE ARQUIVOS DESEJADO :\nDigite:\n");

    for (i = 0; i < 4; i++)
    {
        arq_base[i] = tamanhoArquivo / base[i];
        tamanhoSubArquivos[0] = arq_base[i] / tamanhoRegistro;
        tamanhoSubArquivos[1] = totalRegistros - (base[i] - 1) * (arq_base[i] / tamanhoRegistro);
        printf("\t( %d ) Para %02d arquivos de tamanho %d (%d arquivos com %d Registros e mais 1 com %d Registros) \n", i + 1, base[i], arq_base[i], base[i] - 1, tamanhoSubArquivos[0], tamanhoSubArquivos[1]);
    }
    printf("==> ");
    scanf("%d", &resposta);
    if (resposta > 4)
    {
        return 1;
    }

    // Transformar resposta em indice dos array (j)
    // Obter os valores para usar no For, e gerar os totais de registros, por arquivo
    j = resposta - 1;
    tamanhoSubArquivos[0] = arq_base[j] / tamanhoRegistro;
    tamanhoSubArquivos[1] = totalRegistros - (base[j] - 1) * (arq_base[j] / tamanhoRegistro);

    //////////////////////////////////////////////////
    // 3. Geracao de n arquivos, com base no principal
    //////////////////////////////////////////////////
    for (i = 1; i <= base[j]; i++)
    {
        sprintf(nomeArquivo, "arq-%03d.dat", i);
        g = fopen(nomeArquivo, "w");
        if (i != j)
        {
            n = (Endereco *)malloc(tamanhoSubArquivos[0] * tamanhoRegistro);
            fread(n, tamanhoRegistro, tamanhoSubArquivos[0], f);
            fwrite(n, tamanhoRegistro, tamanhoSubArquivos[0], g);
        }
        else
        {
            n = (Endereco *)malloc(tamanhoSubArquivos[1] * tamanhoRegistro);
            fread(n, tamanhoRegistro, tamanhoSubArquivos[1], f);
            fwrite(n, tamanhoRegistro, tamanhoSubArquivos[1], g);
        }
        free(n);
        fclose(g);
        g = fopen(nomeArquivo, "r");
        fseek(g, 0, SEEK_END);
        posicao = ftell(g); // posicao dando tamanho diferente...
        rewind(g);
        fclose(g);
    }
    fclose(f);

    printf("(5) DESEJA ORDENAR POR QSORT OS %d ARQUIVOS ?\nDigite:\n", base[j]);
    printf("\t( 1 ) Sim\n\t( 2 ) Nao\n==> ");
    scanf("%d", &resposta);
    if (resposta > 1)
    {
        return 1;
    }

    //////////////////////////////////////////////////////
    // 4. Cria os mesmos arquivos, mas ordenados por qsort
    //////////////////////////////////////////////////////
    for (i = 1; i <= base[j]; i++)
    {
        sprintf(nomeArquivo1, "arq-%03d.dat", i); // arquivo de entrada
        g = fopen(nomeArquivo1, "r");
        sprintf(nomeArquivo2, "arq-ordenado-%03d.dat", i); // arquivo de saida
        h = fopen(nomeArquivo2, "w");
        fseek(g, 0, SEEK_END);
        posicao = ftell(g);
        n = (Endereco *)malloc(posicao);
        rewind(g);
        if (i != j)
        {
            fread(n, tamanhoRegistro, tamanhoSubArquivos[0], g);
            qsort(n, tamanhoSubArquivos[0], tamanhoRegistro, compara);
            fwrite(n, tamanhoRegistro, tamanhoSubArquivos[0], h);
        }
        else
        {
            fread(n, tamanhoRegistro, tamanhoSubArquivos[1], g);
            qsort(n, tamanhoSubArquivos[1], tamanhoRegistro, compara);
            fwrite(n, tamanhoRegistro, tamanhoSubArquivos[1], h);
        }
        fclose(h);
        fclose(g);
        free(n);
    }
    printf("%d Arquivos ordenados por QSORT!\n\n", base[j]);
    printf("(6) DESEJA GERAR O ARQUIVO UNIFICADO FINAL, E ORDENADO ?\nDigite:\n");
    printf("\t( 1 ) Sim\n\t( 2 ) Nao\n==> ");
    scanf("%d", &resposta);
    if (resposta > 1)
    {
        return 1;
    }

    ////////////////////////////////////////////////////
    // 5. Geracao do arquivo unificado, ordenado e final
    ////////////////////////////////////////////////////
    c = base[j];
    i = 1;
    while (c < (2 * base[j] - 1))
    {
        c++;
        sprintf(nomeArquivo, "arq-ordenado-%03d.dat", i);
        f = fopen(nomeArquivo, "r");
        sprintf(nomeArquivo1, "arq-ordenado-%03d.dat", i + 1);
        g = fopen(nomeArquivo1, "r");
        sprintf(nomeArquivo2, "arq-ordenado-%03d.dat", c);
        h = fopen(nomeArquivo2, "w");
        fread(&ea, tamanhoRegistro, 1, f);
        fread(&eb, tamanhoRegistro, 1, g);
        while (!feof(f) && !feof(g))
        {
            if (compara(&ea, &eb) < 0) // ea.cep < eb.cep
            {
                fwrite(&ea, tamanhoRegistro, 1, h);
                fread(&ea, tamanhoRegistro, 1, f);
            }
            else
            {
                fwrite(&eb, tamanhoRegistro, 1, h);
                fread(&eb, tamanhoRegistro, 1, g);
            }
        }
        while (!feof(f))
        {
            fwrite(&ea, tamanhoRegistro, 1, h);
            fread(&ea, tamanhoRegistro, 1, f);
        }
        while (!feof(g))
        {
            fwrite(&eb, tamanhoRegistro, 1, h);
            fread(&eb, tamanhoRegistro, 1, g);
        }
        fclose(f);
        fclose(g);
        fclose(h);
        i+=2;
    }
    sprintf(nomeArquivo1, "arq-ordenado-%03d.dat", c);
    f = fopen(nomeArquivo1, "r");
    fseek(f, 0, SEEK_END);
    posicao = ftell(f);
    n = (Endereco *)malloc(posicao);
    rewind(f);
    fread(n, tamanhoRegistro, totalRegistros, f);
    fclose(f);
    g = fopen("arquivo-final-ordenado.dat", "w");
    fwrite(n, tamanhoRegistro, totalRegistros, g);
    free(n);
    fclose(g);
    printf("\n1 Arquivo final unico gerado, ordenado!\n");
    
    // fim do programa
    return 0;
}