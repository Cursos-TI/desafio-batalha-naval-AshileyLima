#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// =====================================================================
// DEFINIÇÕES DE CONSTANTES E ESTRUTURAS
// =====================================================================

#define TAMANHO_TABULEIRO 10
#define TAMANHO_NAVIO 3
#define TAMANHO_HABILIDADE 7

// Valores para representar diferentes elementos no tabuleiro
#define AGUA 0
#define NAVIO 3
#define AREA_EFEITO 5

// Enumeração para os tipos de orientação dos navios
typedef enum {
    HORIZONTAL,
    VERTICAL,
    DIAGONAL_PRINCIPAL,
    DIAGONAL_SECUNDARIA
} OrientacaoNavio;

// Enumeração para os tipos de habilidades especiais
typedef enum {
    CONE,
    CRUZ,
    OCTAEDRO
} TipoHabilidade;

// Enumeração para os níveis do jogo
typedef enum {
    NIVEL_NOVATO = 1,
    NIVEL_AVENTUREIRO = 2,
    NIVEL_MESTRE = 3,
    SAIR = 4
} NivelJogo;

// Estrutura para representar um navio
typedef struct {
    int linha;
    int coluna;
    OrientacaoNavio orientacao;
    const char* nome;
} Navio;

// Estrutura para representar uma habilidade especial
typedef struct {
    int linha_origem;
    int coluna_origem;
    TipoHabilidade tipo;
    const char* nome;
    int matriz[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE];
} HabilidadeEspecial;

// =====================================================================
// FUNÇÕES UTILITÁRIAS
// =====================================================================

// Função auxiliar para calcular valor absoluto
int abs(int valor) {
    return valor < 0 ? -valor : valor;
}

// Função para limpar a tela (multiplataforma)
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Função para pausar execução
void pausar() {
    printf("\nPressione Enter para continuar...");
    getchar();
    getchar(); // Para capturar o Enter pendente
}

// =====================================================================
// FUNÇÕES BÁSICAS DO TABULEIRO
// =====================================================================

// Função para inicializar o tabuleiro com água
void inicializarTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("Inicializando tabuleiro 10x10...\n");
    
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            tabuleiro[i][j] = AGUA;
        }
    }
    
    printf("Tabuleiro inicializado com sucesso!\n\n");
}

// Função para obter caractere visual baseado no valor
char obterCaractereVisual(int valor) {
    switch (valor) {
        case AGUA: return '~';
        case NAVIO: return '#';
        case AREA_EFEITO: return '*';
        default: return '?';
    }
}

// Função para exibir o tabuleiro com caracteres visuais
void exibirTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("=== TABULEIRO DE BATALHA NAVAL ===\n");
    printf("Legenda: ~ = Água, # = Navio, * = Área de Efeito\n\n");
    
    // Cabeçalho com números das colunas
    printf("   ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
        printf("%2d ", j);
    }
    printf("\n");
    
    // Linha separadora
    printf("   ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
        printf("---");
    }
    printf("\n");
    
    // Exibe cada linha do tabuleiro
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf("%2d|", i);
        
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            char caractere = obterCaractereVisual(tabuleiro[i][j]);
            printf(" %c ", caractere);
        }
        printf("\n");
    }
    printf("\n");
}

// Função para exibir tabuleiro com valores numéricos
void exibirTabuleiroNumerico(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("=== TABULEIRO NUMÉRICO ===\n");
    printf("Valores: 0 = Água, 3 = Navio, 5 = Área de Efeito\n\n");
    
    // Cabeçalho com números das colunas
    printf("   ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
        printf("%2d ", j);
    }
    printf("\n");
    
    // Linha separadora
    printf("   ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
        printf("---");
    }
    printf("\n");
    
    // Exibe cada linha do tabuleiro
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf("%2d|", i);
        
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            printf("%2d ", tabuleiro[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// =====================================================================
// FUNÇÕES DE VALIDAÇÃO E POSICIONAMENTO DE NAVIOS
// =====================================================================

// Função para obter o nome da orientação como string
const char* obterNomeOrientacao(OrientacaoNavio orientacao) {
    switch (orientacao) {
        case HORIZONTAL: return "Horizontal";
        case VERTICAL: return "Vertical";
        case DIAGONAL_PRINCIPAL: return "Diagonal Principal (\\)";
        case DIAGONAL_SECUNDARIA: return "Diagonal Secundária (/)";
        default: return "Desconhecida";
    }
}

// Função para validar se a posição está dentro dos limites
bool posicaoValida(int linha, int coluna, int tamanho, OrientacaoNavio orientacao) {
    if (linha < 0 || linha >= TAMANHO_TABULEIRO || coluna < 0 || coluna >= TAMANHO_TABULEIRO) {
        return false;
    }
    
    switch (orientacao) {
        case HORIZONTAL:
            return (coluna + tamanho <= TAMANHO_TABULEIRO);
        case VERTICAL:
            return (linha + tamanho <= TAMANHO_TABULEIRO);
        case DIAGONAL_PRINCIPAL:
            return (linha + tamanho <= TAMANHO_TABULEIRO && coluna + tamanho <= TAMANHO_TABULEIRO);
        case DIAGONAL_SECUNDARIA:
            return (linha + tamanho <= TAMANHO_TABULEIRO && coluna - tamanho + 1 >= 0);
        default:
            return false;
    }
}

// Função para calcular posição do navio baseada na orientação
void calcularPosicaoNavio(int linha_inicial, int coluna_inicial, OrientacaoNavio orientacao, 
                         int indice, int* linha_resultado, int* coluna_resultado) {
    switch (orientacao) {
        case HORIZONTAL:
            *linha_resultado = linha_inicial;
            *coluna_resultado = coluna_inicial + indice;
            break;
        case VERTICAL:
            *linha_resultado = linha_inicial + indice;
            *coluna_resultado = coluna_inicial;
            break;
        case DIAGONAL_PRINCIPAL:
            *linha_resultado = linha_inicial + indice;
            *coluna_resultado = coluna_inicial + indice;
            break;
        case DIAGONAL_SECUNDARIA:
            *linha_resultado = linha_inicial + indice;
            *coluna_resultado = coluna_inicial - indice;
            break;
    }
}

// Função para verificar sobreposição de navios
bool verificarSobreposicao(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], 
                          int linha, int coluna, int tamanho, OrientacaoNavio orientacao) {
    for (int i = 0; i < tamanho; i++) {
        int linha_atual, coluna_atual;
        calcularPosicaoNavio(linha, coluna, orientacao, i, &linha_atual, &coluna_atual);
        
        if (tabuleiro[linha_atual][coluna_atual] == NAVIO) {
            return true;
        }
    }
    return false;
}

// Função para posicionar um navio no tabuleiro
bool posicionarNavio(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], Navio navio) {
    printf("Posicionando %s na posição (%d,%d) - %s\n", 
           navio.nome, navio.linha, navio.coluna, obterNomeOrientacao(navio.orientacao));
    
    if (!posicaoValida(navio.linha, navio.coluna, TAMANHO_NAVIO, navio.orientacao)) {
        printf("ERRO: Posição inválida para %s!\n", navio.nome);
        return false;
    }
    
    if (verificarSobreposicao(tabuleiro, navio.linha, navio.coluna, TAMANHO_NAVIO, navio.orientacao)) {
        printf("ERRO: %s se sobrepõe a outro navio!\n", navio.nome);
        return false;
    }
    
    for (int i = 0; i < TAMANHO_NAVIO; i++) {
        int linha_atual, coluna_atual;
        calcularPosicaoNavio(navio.linha, navio.coluna, navio.orientacao, i, &linha_atual, &coluna_atual);
        tabuleiro[linha_atual][coluna_atual] = NAVIO;
    }
    
    printf("%s posicionado com sucesso!\n\n", navio.nome);
    return true;
}

// =====================================================================
// FUNÇÕES DE HABILIDADES ESPECIAIS (NÍVEL MESTRE)
// =====================================================================

// Função para criar a matriz de habilidade em formato de CONE
void criarHabilidadeCone(HabilidadeEspecial* habilidade) {
    // Inicializa toda a matriz com 0
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            habilidade->matriz[i][j] = 0;
        }
    }
    
    int centro = TAMANHO_HABILIDADE / 2;
    
    // Cria o padrão do cone usando loops aninhados e condicionais
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            int distancia_horizontal = abs(j - centro);
            if (distancia_horizontal <= i && i <= centro + 2) {
                habilidade->matriz[i][j] = 1;
            }
        }
    }
}

// Função para criar a matriz de habilidade em formato de CRUZ
void criarHabilidadeCruz(HabilidadeEspecial* habilidade) {
    // Inicializa toda a matriz com 0
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            habilidade->matriz[i][j] = 0;
        }
    }
    
    int centro = TAMANHO_HABILIDADE / 2;
    
    // Cria o padrão da cruz usando loops aninhados e condicionais
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            if (i == centro || j == centro) {
                habilidade->matriz[i][j] = 1;
            }
        }
    }
}

// Função para criar a matriz de habilidade em formato de OCTAEDRO
void criarHabilidadeOctaedro(HabilidadeEspecial* habilidade) {
    // Inicializa toda a matriz com 0
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            habilidade->matriz[i][j] = 0;
        }
    }
    
    int centro = TAMANHO_HABILIDADE / 2;
    
    // Cria o padrão do octaedro usando loops aninhados e condicionais
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            int distancia_linha = abs(i - centro);
            int distancia_coluna = abs(j - centro);
            
            if (distancia_linha + distancia_coluna <= centro) {
                habilidade->matriz[i][j] = 1;
            }
        }
    }
}

// Função para exibir a matriz de uma habilidade
void exibirMatrizHabilidade(HabilidadeEspecial* habilidade) {
    printf("\n=== MATRIZ DA HABILIDADE %s ===\n", habilidade->nome);
    
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            printf("%d ", habilidade->matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Função para aplicar habilidade ao tabuleiro
void aplicarHabilidade(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], HabilidadeEspecial* habilidade) {
    printf("Aplicando habilidade %s na posição (%d,%d)...\n", 
           habilidade->nome, habilidade->linha_origem, habilidade->coluna_origem);
    
    int centro_habilidade = TAMANHO_HABILIDADE / 2;
    
    // Percorre toda a matriz de habilidade usando loops aninhados
    for (int i = 0; i < TAMANHO_HABILIDADE; i++) {
        for (int j = 0; j < TAMANHO_HABILIDADE; j++) {
            // Condicional para verificar se a posição está na área de efeito
            if (habilidade->matriz[i][j] == 1) {
                int linha_tabuleiro = habilidade->linha_origem + (i - centro_habilidade);
                int coluna_tabuleiro = habilidade->coluna_origem + (j - centro_habilidade);
                
                // Condicional para verificar limites do tabuleiro
                if (linha_tabuleiro >= 0 && linha_tabuleiro < TAMANHO_TABULEIRO &&
                    coluna_tabuleiro >= 0 && coluna_tabuleiro < TAMANHO_TABULEIRO) {
                    
                    // Condicional para não sobrescrever navios
                    if (tabuleiro[linha_tabuleiro][coluna_tabuleiro] != NAVIO) {
                        tabuleiro[linha_tabuleiro][coluna_tabuleiro] = AREA_EFEITO;
                    }
                }
            }
        }
    }
    
    printf("Habilidade %s aplicada com sucesso!\n\n", habilidade->nome);
}

// =====================================================================
// FUNÇÕES DOS NÍVEIS DO JOGO
// =====================================================================

// NÍVEL NOVATO: 2 navios (horizontal e vertical)
void executarNivelNovato() {
    limparTela();
    printf("=== NÍVEL NOVATO ===\n");
    printf("Posicionamento básico de 2 navios\n\n");
    
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tabuleiro);
    
    // Definir 2 navios: horizontal e vertical
    Navio navios[2] = {
        {2, 1, HORIZONTAL, "Navio 1 (Horizontal)"},
        {5, 7, VERTICAL, "Navio 2 (Vertical)"}
    };
    
    printf("=== POSICIONAMENTO DOS NAVIOS ===\n");
    bool sucesso = true;
    for (int i = 0; i < 2; i++) {
        if (!posicionarNavio(tabuleiro, navios[i])) {
            sucesso = false;
            break;
        }
    }
    
    if (sucesso) {
        exibirTabuleiro(tabuleiro);
        exibirTabuleiroNumerico(tabuleiro);
        
        printf("=== RESUMO DO NÍVEL NOVATO ===\n");
        printf("✅ 2 navios posicionados com sucesso!\n");
        printf("✅ Orientações: Horizontal e Vertical\n");
        printf("✅ Validação de limites implementada\n");
        printf("✅ Detecção de sobreposição ativa\n");
    } else {
        printf("❌ Erro no posicionamento dos navios!\n");
    }
    
    pausar();
}

// NÍVEL AVENTUREIRO: 4 navios (incluindo diagonais)
void executarNivelAventureiro() {
    limparTela();
    printf("=== NÍVEL AVENTUREIRO ===\n");
    printf("Posicionamento avançado de 4 navios com orientações diagonais\n\n");
    
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tabuleiro);
    
    // Definir 4 navios com diferentes orientações
    Navio navios[4] = {
        {2, 1, HORIZONTAL, "Fragata (Horizontal)"},
        {5, 7, VERTICAL, "Destroyer (Vertical)"},
        {0, 4, DIAGONAL_PRINCIPAL, "Cruzador (Diagonal \\)"},
        {6, 5, DIAGONAL_SECUNDARIA, "Submarino (Diagonal /)"}
    };
    
    printf("=== POSICIONAMENTO DOS NAVIOS ===\n");
    bool sucesso = true;
    for (int i = 0; i < 4; i++) {
        if (!posicionarNavio(tabuleiro, navios[i])) {
            sucesso = false;
            break;
        }
    }
    
    if (sucesso) {
        exibirTabuleiro(tabuleiro);
        exibirTabuleiroNumerico(tabuleiro);
        
        printf("=== RESUMO DO NÍVEL AVENTUREIRO ===\n");
        printf("✅ 4 navios posicionados com sucesso!\n");
        printf("✅ Orientações: Horizontal, Vertical e 2 Diagonais\n");
        printf("✅ Sistema de orientação avançado implementado\n");
        printf("✅ Algoritmos de posicionamento diagonal funcionais\n");
        
        printf("\n=== DETALHES DOS NAVIOS ===\n");
        for (int i = 0; i < 4; i++) {
            printf("%s: (%d,%d) - %s\n", 
                   navios[i].nome, navios[i].linha, navios[i].coluna, 
                   obterNomeOrientacao(navios[i].orientacao));
        }
    } else {
        printf("❌ Erro no posicionamento dos navios!\n");
    }
    
    pausar();
}

// NÍVEL MESTRE: 4 navios + 3 habilidades especiais
void executarNivelMestre() {
    limparTela();
    printf("=== NÍVEL MESTRE ===\n");
    printf("Sistema completo com navios e habilidades especiais\n\n");
    
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tabuleiro);
    
    // Definir 4 navios
    Navio navios[4] = {
        {1, 1, HORIZONTAL, "Fragata (Horizontal)"},
        {6, 8, VERTICAL, "Destroyer (Vertical)"},
        {0, 5, DIAGONAL_PRINCIPAL, "Cruzador (Diagonal \\)"},
        {8, 4, DIAGONAL_SECUNDARIA, "Submarino (Diagonal /)"}
    };
    
    // Posicionar navios
    printf("=== POSICIONAMENTO DOS NAVIOS ===\n");
    bool sucesso = true;
    for (int i = 0; i < 4; i++) {
        if (!posicionarNavio(tabuleiro, navios[i])) {
            sucesso = false;
            break;
        }
    }
    
    if (!sucesso) {
        printf("❌ Erro no posicionamento dos navios!\n");
        pausar();
        return;
    }
    
    // Criar e configurar habilidades especiais
    printf("=== CRIANDO HABILIDADES ESPECIAIS ===\n");
    HabilidadeEspecial habilidades[3];
    
    // Habilidade 1: CONE
    habilidades[0].linha_origem = 2;
    habilidades[0].coluna_origem = 5;
    habilidades[0].tipo = CONE;
    habilidades[0].nome = "CONE";
    printf("Criando habilidade CONE...\n");
    criarHabilidadeCone(&habilidades[0]);
    
    // Habilidade 2: CRUZ
    habilidades[1].linha_origem = 7;
    habilidades[1].coluna_origem = 2;
    habilidades[1].tipo = CRUZ;
    habilidades[1].nome = "CRUZ";
    printf("Criando habilidade CRUZ...\n");
    criarHabilidadeCruz(&habilidades[1]);
    
    // Habilidade 3: OCTAEDRO
    habilidades[2].linha_origem = 4;
    habilidades[2].coluna_origem = 7;
    habilidades[2].tipo = OCTAEDRO;
    habilidades[2].nome = "OCTAEDRO";
    printf("Criando habilidade OCTAEDRO...\n");
    criarHabilidadeOctaedro(&habilidades[2]);
    
    printf("Todas as habilidades criadas com sucesso!\n\n");
    
    // Exibir matrizes das habilidades
    printf("=== MATRIZES DAS HABILIDADES ===\n");
    for (int i = 0; i < 3; i++) {
        exibirMatrizHabilidade(&habilidades[i]);
    }
    
    // Aplicar habilidades ao tabuleiro
    printf("=== APLICANDO HABILIDADES ===\n");
    for (int i = 0; i < 3; i++) {
        aplicarHabilidade(tabuleiro, &habilidades[i]);
    }
    
    // Exibir resultado final
    printf("=== RESULTADO FINAL ===\n");
    exibirTabuleiro(tabuleiro);
    exibirTabuleiroNumerico(tabuleiro);
    
    // Estatísticas finais
    int contador_agua = 0, contador_navio = 0, contador_efeito = 0;
    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++) {
            switch (tabuleiro[i][j]) {
                case AGUA: contador_agua++; break;
                case NAVIO: contador_navio++; break;
                case AREA_EFEITO: contador_efeito++; break;
            }
        }
    }
    
    printf("=== RESUMO DO NÍVEL MESTRE ===\n");
    printf("✅ 4 navios posicionados com sucesso!\n");
    printf("✅ 3 habilidades especiais aplicadas!\n");
    printf("✅ Tipos de habilidade: CONE, CRUZ, OCTAEDRO\n");
    printf("✅ Sistema de área de efeito implementado\n");
    printf("\n=== ESTATÍSTICAS FINAIS ===\n");
    printf("Posições com água: %d\n", contador_agua);
    printf("Posições com navios: %d\n", contador_navio);
    printf("Posições com área de efeito: %d\n", contador_efeito);
    printf("Total de posições: %d\n", TAMANHO_TABULEIRO * TAMANHO_TABULEIRO);
    
    pausar();
}

// =====================================================================
// SISTEMA DE MENU PRINCIPAL
// =====================================================================

void exibirMenu() {
    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║          BATALHA NAVAL - SISTEMA COMPLETO    ║\n");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║                                              ║\n");
    printf("║  1. NÍVEL NOVATO                             ║\n");
    printf("║     • 2 navios básicos                       ║\n");
    printf("║     • Orientações: Horizontal e Vertical     ║\n");
    printf("║                                              ║\n");
    printf("║  2. NÍVEL AVENTUREIRO                        ║\n");
    printf("║     • 4 navios avançados                     ║\n");
    printf("║     • Orientações: + Diagonais               ║\n");
    printf("║                                              ║\n");
    printf("║  3. NÍVEL MESTRE                             ║\n");
    printf("║     • 4 navios + 3 habilidades especiais     ║\n");
    printf("║     • Áreas de efeito: Cone, Cruz, Octaedro  ║\n");
    printf("║                                              ║\n");
    printf("║  4. SAIR                                     ║\n");
    printf("║                                              ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("\nEscolha uma opção (1-4): ");
}

void exibirCabecalho() {
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║                                                      ║\n");
    printf("║    ⚓ BATALHA NAVAL - SISTEMA COMPLETO ⚓           ║\n");
    printf("║                                                      ║\n");
    printf("║    Desenvolvido para demonstrar progressão em        ║\n");
    printf("║    algoritmos de posicionamento e áreas de efeito    ║\n");
    printf("║                                                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}

// =====================================================================
// FUNÇÃO PRINCIPAL
// =====================================================================

int main() {
    int opcao;
    
    do {
        limparTela();
        exibirCabecalho();
        exibirMenu();
        
        scanf("%d", &opcao);
        
        switch (opcao) {
            case NIVEL_NOVATO:
                executarNivelNovato();
                break;
                
            case NIVEL_AVENTUREIRO:
                executarNivelAventureiro();
                break;
                
            case NIVEL_MESTRE:
                executarNivelMestre();
                break;
                
            case SAIR:
                limparTela();
                printf("╔══════════════════════════════════════════════╗\n");
                printf("║                                              ║\n");
                printf("║    Obrigado por usar o Sistema Batalha Naval ║\n");
                printf("║                                              ║\n");
                printf("║    ⚓ Todos os níveis implementados! ⚓     ║\n");
                printf("║                                              ║\n");
                printf("║    • Nível Novato: Completo                  ║\n");
                printf("║    • Nível Aventureiro: Completo             ║\n");
                printf("║    • Nível Mestre: Completo                  ║\n");
                printf("║                                              ║\n");
                printf("║    Sistema pronto para expansão futura!      ║\n");
                printf("║                                              ║\n");
                printf("╚══════════════════════════════════════════════╝\n");
                break;
                
            default:
                printf("\n❌ Opção inválida! Escolha uma opção de 1 a 4.\n");
                pausar();
                break;
        }
        
    } while (opcao != SAIR);
    
    return 0;
}