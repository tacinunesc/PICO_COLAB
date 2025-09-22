# PICO_COLAB

## 🌡️ Temperatura registrada pelo Pico
O Pico coleta dados de temperatura em tempo real, esses dados foram analisados e visualizados no notebook. 

## 📈 Gráfico de evolução da temperatura
O que mostra:

* A linha representa a variação da temperatura ao longo do tempo

* Picos indicam mudanças bruscas como aquecimento rápido ou resfriamento

* Períodos estáveis aparecem como trechos mais retos da linha
<div align="center">
  <img src="temperatura pico.png "  alt="Controle" width="70%">

</div>

## CODIGO
```python
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Carrega o CSV
df = pd.read_csv("bitdoglab.csv")

# Converte valores numéricos
df["Valor"] = pd.to_numeric(df["Valor"], errors="coerce")
df.dropna(subset=["Valor"], inplace=True)

# Converte timestamp se necessário
if "Timestamp" in df.columns:
    df["Timestamp"] = pd.to_datetime(df["Timestamp"], errors="coerce")

df_temp = df[df["Tipo"] == "Temperatura"]

plt.figure(figsize=(10, 5))
plt.plot(df_temp["Timestamp"], df_temp["Valor"], color="darkorange")
plt.title("Temperatura registrada pela Pico")
plt.xlabel("Tempo")
plt.ylabel("Temperatura (°C)")
plt.grid(True)
plt.tight_layout()
plt.show()
```


## 🌡️ Temperatura ao logo do tempo
Gráfico para visualizar a temperatura ao longo do tempo, destacando os valores normais e os outliers (valores anormais)

## 🔍 Separação dos dados
* Outliers: temperaturas fora dos limites

* Normais: temperaturas dentro dos limites

## Definições 
* Pontos normais: em azul claro , representando temperaturas dentro do intervalo esperado.

* Outliers: em vermelho (red), com marcador "x", indicando temperaturas anormais.

<div align="center">
  <img src="temp.png "  alt="Controle" width="70%">

</div>
## CODIGO

```python
# Cálculo do IQR e limites
q1 = df_temp["Valor"].quantile(0.25)
q3 = df_temp["Valor"].quantile(0.75)
iqr = q3 - q1
limite_inferior = q1 - 1.5 * iqr
limite_superior = q3 + 1.5 * iqr

# Identifica outliers
outliers = df_temp[(df_temp["Valor"] < limite_inferior) | (df_temp["Valor"] > limite_superior)]
normais = df_temp[(df_temp["Valor"] >= limite_inferior) & (df_temp["Valor"] <= limite_superior)]

# Gráfico de dispersão com destaque
plt.figure(figsize=(12, 6))

# Pontos normais
plt.scatter(normais["Timestamp"], normais["Valor"], color="skyblue", label="Temperatura normal")

# Pontos outliers
plt.scatter(outliers["Timestamp"], outliers["Valor"], color="red", label="anormal", marker="x", s=80)

# Faixa de referência
plt.axhline(limite_inferior, color="green", linestyle="--", label=f"Limite inferior: {limite_inferior:.1f}°C")
plt.axhline(limite_superior, color="orange", linestyle="--", label=f"Limite superior: {limite_superior:.1f}°C")

# Personalização
plt.title("Temperatura ao longo do tempo")
plt.xlabel("Timestamp")
plt.ylabel("Temperatura (°C)")
plt.xticks(rotation=45)
plt.legend()
plt.tight_layout()
plt.grid(True)
plt.show()
```




## Análise de Eventos

Análise dos eventos registrados em um arquivo CSV chamado bitdoglab.csv

## O que ele faz:
* Lê o arquivo com os dados
* Filtra só os eventos (ignora outros tipos de informação)
* Conta quantas vezes cada evento aparece.
* Mostra um gráfico de barras com esses eventos e suas quantidades

Neste caso foram coletados os dados do joystick, botões e buzzer que são acionados na placa, fora a temperatura interna

<div align="center">
  <img src="classificação de eventos.png "  alt="Controle" width="70%">

</div>

## CODIGO

```python
# Carrega o CSV
df = pd.read_csv("bitdoglab.csv")

# Padroniza nomes de colunas e valores
df.columns = df.columns.str.strip()
df["Tipo"] = df["Tipo"].astype(str).str.strip().str.upper()
df["Valor"] = df["Valor"].astype(str).str.strip().str.upper()

# Filtra apenas os registros onde Tipo é 'EVENTO'
df_eventos = df[df["Tipo"] == "EVENTO"]

# Conta os eventos distintos na coluna Valor
tabela_classificacao = df_eventos["Valor"].value_counts().reset_index()
tabela_classificacao.columns = ["Evento", "Quantidade"]

# Exibe a tabela
print("📋 Classificação dos eventos:")
print(tabela_classificacao)

# Gera gráfico de barras
plt.figure(figsize=(10, 6))
sns.barplot(data=tabela_classificacao, x="Evento", y="Quantidade", palette="Set2")
plt.title("Classificação dos eventos registrados")
plt.xlabel("Evento")
plt.ylabel("Quantidade")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.show()

```
