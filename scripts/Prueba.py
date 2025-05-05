import subprocess
import time
import os
import csv
import matplotlib.pyplot as plt
import signal

NUM_EPISODES = 500
WAIT_BETWEEN_EPISODES = 2  # segundos
LOG_FILENAME = "training_log.csv"

# Inicializar archivo de log
def init_log():
    with open(LOG_FILENAME, "w", newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Episodio", "Recompensa"])

# Agregar recompensa al log
def log_reward(ep, reward):
    with open(LOG_FILENAME, "a", newline='') as f:
        writer = csv.writer(f)
        writer.writerow([ep, reward])

# Graficar los resultados finales
def plot_rewards():
    episodios = []
    recompensas = []
    with open(LOG_FILENAME, "r") as f:
        reader = csv.reader(f)
        next(reader)  # skip header
        for row in reader:
            episodios.append(int(row[0]))
            recompensas.append(float(row[1]))

    plt.plot(episodios, recompensas)
    plt.xlabel("Episodio")
    plt.ylabel("Recompensa")
    plt.title("Recompensa por Episodio - Q-Learning")
    plt.grid()
    plt.savefig("training_progress.png")
    plt.show()

# Ejecutar un episodio de entrenamiento con los dos agentes
def run_episode(ep):
    print(f"\n🚀 Iniciando episodio {ep + 1}/{NUM_EPISODES}")

    # Lanzar EnemyBot.py
    bot_proc = subprocess.Popen(
        ["python", "EnemyBot.py", "2"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )

    # Lanzar Raijin_Stormbot.py y capturar recompensa
    agent_proc = subprocess.Popen(
        ["python", "Raijin_Stormbot.py", "1"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    total_reward = 0
    for line in agent_proc.stdout:
        print(line.strip())
        if "🏁 Recompensa total:" in line:
            try:
                total_reward = float(line.strip().split(":")[1].split()[0])
            except:
                total_reward = 0

    agent_proc.wait()
    try:
        bot_proc.send_signal(signal.SIGTERM)
    except Exception:
        pass

    log_reward(ep + 1, total_reward)
    print(f"✅ Episodio {ep + 1} terminado con recompensa {total_reward}")
    time.sleep(WAIT_BETWEEN_EPISODES)

def main():
    print("🧠 Entrenamiento automático iniciado")
    print("🔗 Asegurate de que el simulador ./testcase esté corriendo en Ubuntu (IP correcta en Configuration.py)")
    init_log()

    for ep in range(NUM_EPISODES):
        try:
            run_episode(ep)
        except Exception as e:
            print(f"⚠️ Error en episodio {ep + 1}: {e}")
            time.sleep(3)

    print("📈 Generando gráfico de entrenamiento...")
    plot_rewards()
    print("🏁 Entrenamiento finalizado.")

if __name__ == "__main__":
    main()