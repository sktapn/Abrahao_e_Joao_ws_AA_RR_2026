import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import glob
import sys
import os

# ─────────────────────────────────────────────
# CONFIGURAÇÃO
# ─────────────────────────────────────────────
NOMES_TIPO = {
    "crescente":   "Crescente",
    "decrescente": "Decrescente",
    "aleatorio":   "Aleatório",
}

CORES = {
    "crescente":   "#2196F3",  # azul
    "decrescente": "#F44336",  # vermelho
    "aleatorio":   "#4CAF50",  # verde
}

MARCADORES = {
    "crescente":   "o",
    "decrescente": "s",
    "aleatorio":   "^",
}

# ─────────────────────────────────────────────
# LEITURA DOS CSVs
# ─────────────────────────────────────────────
def carregar_dados():
    """
    Lê todos os arquivos resultados_hash_*.csv no diretório atual
    e retorna um dicionário { tipo: DataFrame } com apenas as linhas de MÉDIA.
    """
    arquivos = glob.glob("resultados_hash_*.csv")

    if not arquivos:
        print("Erro: nenhum arquivo 'resultados_hash_*.csv' encontrado.")
        print("Certifique-se de rodar este script na mesma pasta dos CSVs.")
        sys.exit(1)

    dados = {}
    for arquivo in arquivos:
        # Extrai o tipo do nome do arquivo (ex: "crescente")
        tipo = arquivo.replace("resultados_hash_", "").replace(".csv", "")

        df = pd.read_csv(arquivo)

        # Filtra apenas as linhas de média
        df_media = df[df["execucao"] == "MEDIA"].copy()
        df_media["n_elementos"]  = df_media["n_elementos"].astype(int)
        df_media["tempo_s"]      = df_media["tempo_s"].astype(float)
        df_media["comparacoes"]  = df_media["comparacoes"].astype(float)
        df_media["memoria_kb"]   = df_media["memoria_kb"].astype(float)
        df_media = df_media.sort_values("n_elementos")

        dados[tipo] = df_media
        print(f"  Carregado: {arquivo}  ({len(df_media)} tamanhos de entrada)")

    return dados

# ─────────────────────────────────────────────
# FUNÇÃO GENÉRICA DE PLOT
# ─────────────────────────────────────────────
def plotar_grafico(dados, coluna_y, titulo, label_y, nome_arquivo, formato_y=None):
    fig, ax = plt.subplots(figsize=(9, 5))

    for tipo, df in sorted(dados.items()):
        nome   = NOMES_TIPO.get(tipo, tipo)
        cor    = CORES.get(tipo, "#888888")
        marca  = MARCADORES.get(tipo, "o")

        ax.plot(
            df["n_elementos"],
            df[coluna_y],
            label=nome,
            color=cor,
            marker=marca,
            linewidth=2,
            markersize=7,
        )

    ax.set_title(titulo, fontsize=14, fontweight="bold", pad=12)
    ax.set_xlabel("Número de Elementos", fontsize=12)
    ax.set_ylabel(label_y, fontsize=12)
    ax.legend(fontsize=11)
    ax.grid(True, linestyle="--", alpha=0.5)

    # Formata o eixo X com separador de milhar
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{int(x):,}".replace(",", ".")))

    # Formatação opcional do eixo Y
    if formato_y == "cientifico":
        ax.yaxis.set_major_formatter(ticker.ScalarFormatter(useMathText=True))
        ax.ticklabel_format(style="sci", axis="y", scilimits=(0, 0))
    elif formato_y == "milhar":
        ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{int(x):,}".replace(",", ".")))

    plt.tight_layout()
    plt.savefig(nome_arquivo, dpi=150)
    plt.close()
    print(f"  Salvo: {nome_arquivo}")

# ─────────────────────────────────────────────
# MAIN
# ─────────────────────────────────────────────
def main():
    print("\n=== Gerador de Gráficos — Tabela Hash ===\n")
    print("Lendo arquivos CSV...")
    dados = carregar_dados()

    print("\nGerando gráficos...")

    # 1. Número de elementos × Comparações
    plotar_grafico(
        dados,
        coluna_y    = "comparacoes",
        titulo      = "Tabela Hash — Elementos × Comparações (média de 13 execuções)",
        label_y     = "Comparações",
        nome_arquivo= "grafico_comparacoes.png",
        formato_y   = "milhar",
    )

    # 2. Número de elementos × Tempo de execução
    plotar_grafico(
        dados,
        coluna_y    = "tempo_s",
        titulo      = "Tabela Hash — Elementos × Tempo de Execução (média de 13 execuções)",
        label_y     = "Tempo (segundos)",
        nome_arquivo= "grafico_tempo.png",
        formato_y   = "cientifico",
    )

    # 3. Número de elementos × Memória
    plotar_grafico(
        dados,
        coluna_y    = "memoria_kb",
        titulo      = "Tabela Hash — Elementos × Consumo de Memória (média de 13 execuções)",
        label_y     = "Memória (KB)",
        nome_arquivo= "grafico_memoria.png",
        formato_y   = "milhar",
    )

    print("\nPronto! Arquivos gerados:")
    print("  grafico_comparacoes.png")
    print("  grafico_tempo.png")
    print("  grafico_memoria.png")

main()