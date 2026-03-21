# AnoyingPC Linux - Estrutura de Ficheiros

## Descrição Geral

AnoyingPC Linux é uma versão em C pura do clássico programa de pranks do Windows, portado para Linux com a mesma filosofia de pranks inofensivos e controláveis.

---

## 📁 Ficheiros Incluídos

### Ficheiros Principais

| Ficheiro | Descrição |
|----------|-----------|
| **AnoyPC.c** | Código-fonte principal em C (600+ linhas, totalmente comentado) |
| **Makefile** | Sistema de compilação com targets para build, clean, install |
| **setup.sh** | Script de instalação completo (cria diretórios, compila, configura cron) |
| **README.md** | Documentação completa de utilizador |
| **.gitignore** | Configuração Git para ignorar ficheiros compilados |

### Ficheiros Gerados Pela Instalação

Após `./setup.sh`, os seguintes ficheiros são criados em `~/.anoypc/`:

| Ficheiro | Descrição |
|----------|-----------|
| **anoypc** | Binário compilado (executável principal) |
| **run.sh** | Runner interno chamado pelo cron |
| **features.sh** | Menu interativo para toggle de features |
| **test.sh** | Menu para testar pranks individualmente |
| **anoyon.sh** | Script para ativar cron job |
| **anoyoff.sh** | Script para desativar cron job temporariamente |
| **anoydel.sh** | Script para remoção completa |
| **anoypc.log** | Ficheiro de log de execuções |
| **feat_*.on** | Ficheiros marcadores de features (7 no total) |

---

## 🔧 Compilação

### Pré-requisitos

```bash
# Ubuntu/Debian
apt-get update
apt-get install build-essential

# Fedora/RHEL
dnf groupinstall "Development Tools"

# Arch
pacman -S base-devel
```

### Compilar Manualmente

```bash
cd AnoyLinux
make clean     # Limpar builds anteriores
make           # Compilar
```

### Verificar Compilação

```bash
./anoypc --help  # Não existe (programa não tem opções)
./anoypc BELL    # Testar feature específica
./anoypc         # Testar feature aleatória
```

---

## 🚀 Instalação

```bash
# Instalar tudo (cria diretorios, compila, configura cron)
./setup.sh
```

O script fará:
1. ✅ Verificar contexto de utilizador
2. ✅ Verificar ferramentas de compilação (gcc, make)
3. ✅ Compilar o programa
4. ✅ Criar `~/.anoypc/`
5. ✅ Criar todos os scripts de controle
6. ✅ Ativar todas as 7 features
7. ✅ Registar cron job (cada 5 minutos)
8. ✅ Criar symlinks em `~/.local/bin/`
9. ✅ Criar log file

---

## 📊 Estrutura de Código

### AnoyPC.c (600+ linhas)

```
┌─ Includes & Definitions (30 linhas)
│  ├─ Headers necessários
│  ├─ Enum de features
│  └─ Array de nomes de features
│
├─ Utility Functions (100 linhas)
│  ├─ log_message()     - Logging de execuções
│  ├─ get_home_dir()    - Detectar home directory
│  └─ is_feature_enabled()  - Verificar marker files
│
├─ Feature Implementations (350+ linhas)
│  ├─ feature_bell()      - Terminal bell
│  ├─ feature_message()   - Random messages
│  ├─ feature_mouse()     - Hide/show cursor
│  ├─ feature_flash()     - Screen flash ANSI
│  ├─ feature_reverse()   - Reverse video text
│  ├─ feature_calendar()  - Calendar anomaly
│  └─ feature_sysinfo()   - Fake system info
│
├─ Dispatcher (50 linhas)
│  ├─ run_feature()       - Execute specific feature
│  └─ run_random_feature() - Select & run random
│
└─ Main Function (30 linhas)
   ├─ Parse arguments
   ├─ Verify installation
   └─ Execute feature
```

---

## 🎭 As 7 Funcionalidades

### 1. Terminal Bell (`BELL`)
- **O que faz:** Emite 5 bips sonoros altos + mensagem fake de alerta
- **Código:** Usa ASCII BEL (`\a`)
- **Duração:** ~1 segundo

### 2. Random Message (`MESSAGE`)
- **O que faz:** Mostra mensagem random de 10 opções diferentes
- **Formato:** Caixa decorativa com mensagem assustadora
- **Duração:** 3 segundos

### 3. Mouse Hide/Show (`MOUSE`)
- **O que faz:** Move cursor para canto (0,0) ou centro (50%, 50%)
- **Dependência:** Requer `xdotool` instalado
- **Fallback:** Funciona mesmo sem X11
- **Duração:** 2 segundos

### 4. Screen Flash (`FLASH`)
- **O que faz:** Pisca cores alternadas (vermelho/amarelo) no terminal
- **Código:** Usa sequences ANSI escape
- **Duração:** ~1 segundo

### 5. Reverse Video (`REVERSE`)
- **O que faz:** Mostra texto em modo reverse (branco em preto)
- **Código:** `\033[7m` (reverse) e `\033[0m` (reset)
- **Duração:** 3 segundos

### 6. Calendar Joke (`CALENDAR`)
- **O que faz:** Mostra "anomalia temporal" comparando datas
- **Formato:** Caixa decorativa com aviso de inconsistência
- **Duração:** 3 segundos

### 7. System Info Spoof (`SYSINFO`)
- **O que faz:** Mostra relatório fake de estado do sistema
- **Inclui:** CPU, RAM, Disk, Network, Thermal, Power
- **Duração:** 3 segundos

---

## 📈 Fluxo de Execução

### Ciclo de Instalação

```
setup.sh
├─ Checks (user context, gcc, make)
├─ make clean && make
├─ mkdir ~/.anoypc
├─ cp AnoyPC ~/.anoypc/
├─ touch feat_*.on (7 files)
├─ Create scripts (run.sh, features.sh, test.sh, etc)
├─ crontab -e (register */5 job)
└─ ln -s ~/.local/bin/ (convenience links)
```

### Ciclo de Execução

```
Cada 5 minutos (cron):
├─ ~/.anoypc/run.sh
│  └─ ~/.anoypc/anoypc
│     ├─ Check enabled features (feat_*.on files)
│     ├─ Select random enabled feature
│     ├─ Log execution
│     └─ Execute feature function
│        └─ Print output
└─ Log entry: [2026-03-21 14:05:00] Executing random feature: BELL
```

---

## 🎮 Como Usar

### Após Instalação

```bash
# 1. Ver e alterar features
~/.anoypc/features.sh

# 2. Testar pranks
~/.anoypc/test.sh

# 3. Ver logs
tail -f ~/.anoypc/anoypc.log

# 4. Desativar temporariamente
~/.anoypc/anoyoff.sh

# 5. Reativar
~/.anoypc/anoyon.sh

# 6. Remover completamente
~/.anoypc/anoydel.sh
```

### Usando Symlinks (se PATH está configurado)

```bash
# Se ~/.local/bin está em PATH:
anoypc-features
anoypc-test
```

---

## 🔐 Detalhes Técnicos

### Sistema de Marker Files

Features são controladas por ficheiros simples:

```bash
# Feature ativada
touch ~/.anoypc/feat_BELL.on

# Feature desativada
rm ~/.anoypc/feat_BELL.on

# Verificar status
ls -la ~/.anoypc/feat_*.on
```

### Logging

Todas as execuções são registadas:

```
~/.anoypc/anoypc.log
[2026-03-21 14:00:00] Executing random feature: MESSAGE
[2026-03-21 14:05:00] Executing random feature: FLASH
[2026-03-21 14:10:00] No features enabled. Exiting.
```

### Cron Schedule

```bash
# Ver cron job registado
crontab -l | grep anoypc

# Resultado esperado:
*/5 * * * * ~/.anoypc/run.sh
```

---

## 🛠️ Personalizações

### Alterar Intervalo de Execução

Editar `setup.sh` (linha com `*/5`):

```bash
# A cada 1 minuto:
* * * * * ~/.anoypc/run.sh

# A cada 10 minutos:
*/10 * * * * ~/.anoypc/run.sh

# Duas vezes por hora (0 e 30 minutos):
0,30 * * * * ~/.anoypc/run.sh
```

### Adicionar Nova Feature

1. Editar `AnoyPC.c`:
   - Adicionar função `void feature_my_prank(void)`
   - Adicionar ao enum `FEAT_MYPRANK`
   - Adicionar case no `run_feature()`

2. Recompilar:
   ```bash
   make clean && make
   ./setup.sh
   ```

---

## 📝 Notas

- **Totalmente Seguro:** Sem modificações persistentes no sistema
- **Reversível:** Tudo é desinstalável via `anoydel.sh`
- **Portável:** Funciona em qualquer distro Linux moderna
- **Logging Completo:** Todas as ações registadas
- **Modular:** Features podem ser ativadas/desativadas individualmente
- **Bem Comentado:** Cada função documentada em português/inglês

---

## 📞 Suporte

Se não funcionar:

1. Verificar permissões:
   ```bash
   ls -la ~/.anoypc/
   crontab -l
   ```

2. Ver logs:
   ```bash
   tail -f ~/.anoypc/anoypc.log
   ```

3. Testar manualmente:
   ```bash
   ~/.anoypc/anoypc BELL
   ```

---

**Criado com ❤️ por CreaTico6 - Linux Edition**
