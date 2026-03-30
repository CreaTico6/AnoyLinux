
# 🎭 AnoyPC Linux

> **AnoyPC** é uma suíte minimalista de automação e notificações para sistemas Linux, projetada para simular eventos e efeitos visuais de forma imprevisível e reversível.

---

## ⚡ Instalação Rápida

```bash
git clone git@github.com:CreaTico6/AnoyLinux.git && bash ./AnoyLinux/install.sh
```
> O instalador remove automaticamente o diretório fonte após a configuração.

---

## ✨ Funcionalidades (20 Simulações)

Todas as simulações são reversíveis e seguras:

| Nome            | Descrição Técnica |
|-----------------|------------------|
| **MATRIX**      | Chuva digital em tela cheia com overlay central. |
| **MOUSE_JITTER**| Micro-ajustes aleatórios no cursor. |
| **MOUSE_TELEPORT** | Teleporta o cursor para posição aleatória. |
| **MOUSE_SWAP**  | Inverte botões esquerdo/direito do mouse. |
| **MOUSE_POOP**  | Deixa rastros de quadrados pretos temporários. |
| **CHASING_BUTTON** | Janela X11 que foge do cursor. |
| **CLICK_DISABLE** | Desativa cliques físicos temporariamente. |
| **CUSTOM_CURSOR** | Altera o cursor para ícones bizarros. |
| **BRIGHTNESS_PULSE** | Oscila o brilho da tela via xrandr. |
| **GRAYSCALE**   | Força escala de cinza via xcalib/xrandr. |
| **TERMINAL_LOOP** | Abre terminal, mostra aviso falso e fecha. |
| **BLOCK_SCREEN** | Overlay de foco em tela cheia. |
| **UPSIDE_DOWN** | Rotaciona a tela em 180°. |
| **BELL**        | Testa alertas de áudio. |
| **CAPS_ON**     | Ativa Caps Lock. |
| **ALERT_SCREEN** | Relatório de status no terminal. |
| **SYSINFO**     | Mostra informações falsas do sistema. |
| **FLASH**       | Efeito de flash visual. |
| **CALENDAR**    | Exibe anomalia falsa de calendário. |
| **MESSAGE**     | Mensagens falsas de kernel/dmesg. |

---

## 🚀 Gerenciamento & Controle

Scripts e arquivos de configuração ficam em `~/.anoypc/`:

- `~/.local/bin/anoypc-features`: Menu interativo para ativar/desativar eventos.
- `~/.local/bin/anoypc-test`: Testa eventos individualmente.
- `~/.anoypc/anoyoff.sh`: Suspende o agendador (cron).
- `~/.anoypc/anoyon.sh`: Retoma o agendamento.
- `~/.anoypc/anoydel.sh`: Desinstala completamente a suíte.

---

## 🔧 Visão Técnica

- **Arquitetura:** C99 + X11 (Xlib)
- **Agendamento:** Cron padrão, intervalos imprevisíveis (Stealth Mode)
- **Logs:** `~/.anoypc/anoypc.log`
- **Persistência:** Injeção em `.zshenv`/`.zprofile`

---

## 📦 Dependências Opcionais

Para a feature **GRAYSCALE** funcionar em todos os sistemas, instale:

```bash
sudo apt install xcalib
```
Ou garanta suporte a `xrandr --gamma`.

---

## ⚠️ Aviso Legal

> Projeto para fins educacionais e de teste. Não utilize sem autorização explícita. O autor não se responsabiliza por uso indevido.

---

## 👨‍💻 Autor

Desenvolvido por **tnuno-mo** (Março 2026).


