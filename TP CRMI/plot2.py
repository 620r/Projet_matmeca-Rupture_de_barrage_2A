"""
Détecteur de creux de vague (avant barrage)
===========================================

On détecte la PREMIÈRE baisse brutale de tension
correspondant au creux qui se propage avant la rupture.

3 capteurs espacés de 15 cm.

Usage:
    python wave_back_detector.py fichier.txt
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

# ─────────────────────────────────
# PARAMÈTRES
# ─────────────────────────────────

FICHIER = "Test_kastor_choco_5.txt"

DISTANCE_CM = 15
DISTANCE_M = DISTANCE_CM / 100

SEUIL_RELATIF = 0.05
FENETRE_BASELINE = 1000

SMOOTH_WINDOW = 51
SMOOTH_POLY = 3

PLOT = False

# ─────────────────────────────────


def lire_fichier(path):

    t_lst, a0_lst, a1_lst, a2_lst = [], [], [], []

    in_data = False

    with open(path, "r", encoding="latin-1") as f:

        for line in f:

            line = line.strip()

            if line.startswith("Time"):
                in_data = True
                continue

            if not in_data:
                continue

            if line == "" or line.startswith("Data"):
                continue

            parts = line.replace(",", ".").split()

            if len(parts) >= 4:

                try:

                    t_lst.append(float(parts[0]))
                    a0_lst.append(float(parts[1]))
                    a1_lst.append(float(parts[2]))
                    a2_lst.append(float(parts[3]))

                except ValueError:
                    pass

    return (
        np.array(t_lst),
        np.array(a0_lst),
        np.array(a1_lst),
        np.array(a2_lst),
    )


def lisser(signal):

    w = SMOOTH_WINDOW

    if w % 2 == 0:
        w += 1

    return savgol_filter(signal, w, SMOOTH_POLY)


def detecter_creux(temps, signal, label=""):

    sig_lisse = lisser(signal)

    baseline = np.median(sig_lisse[:FENETRE_BASELINE])

    amplitude = baseline - np.min(sig_lisse)

    seuil = baseline - SEUIL_RELATIF * amplitude

    indices = np.where(sig_lisse < seuil)[0]

    if len(indices) == 0:

        print(f"[{label}] Aucun creux détecté")

        return None

    idx = indices[0]

    t_creux = temps[idx]

    print(
        f"[{label}] baseline={baseline:.4f}  seuil={seuil:.4f}  creux à t={t_creux:.5f}"
    )

    return t_creux, idx, seuil, baseline, sig_lisse


def calculer_vitesse(t1, t2, t3):

    res = {}

    if t1 and t2:
        res["v01"] = DISTANCE_M / (t2 - t1)

    if t2 and t3:
        res["v12"] = DISTANCE_M / (t3 - t2)

    if t1 and t3:
        res["v02"] = 2 * DISTANCE_M / (t3 - t1)

    return res


def afficher(temps, signaux, creux, seuils, baselines, lisses):

    labels = ["AI0", "AI1", "AI2"]

    fig, axes = plt.subplots(3, 1, figsize=(12, 8), sharex=True)

    for i, ax in enumerate(axes):

        ax.plot(temps, signaux[i], alpha=0.3)

        ax.plot(temps, lisses[i])

        ax.axhline(seuils[i], linestyle="--")

        ax.axhline(baselines[i], linestyle=":")

        if creux[i]:
            ax.axvline(creux[i][0])

        ax.set_ylabel(labels[i])

        ax.grid()

    axes[-1].set_xlabel("temps (s)")

    plt.tight_layout()

    plt.show()


def main():

    path = sys.argv[1] if len(sys.argv) > 1 else FICHIER

    print("lecture:", path)

    temps, ai0, ai1, ai2 = lire_fichier(path)

    signaux = [ai0, ai1, ai2]

    creux = []
    seuils = []
    baselines = []
    lisses = []

    for i, sig in enumerate(signaux):

        res = detecter_creux(temps, sig, f"AI{i}")

        if res:

            t, idx, s, b, l = res

            creux.append((t, idx))

            seuils.append(s)

            baselines.append(b)

            lisses.append(l)

        else:

            creux.append(None)

            seuils.append(0)

            baselines.append(0)

            lisses.append(sig)

    t1 = creux[0][0] if creux[0] else None
    t2 = creux[1][0] if creux[1] else None
    t3 = creux[2][0] if creux[2] else None

    vitesses = calculer_vitesse(t1, t2, t3)

    print("\nVitesses")

    for k, v in vitesses.items():

        print(k, "=", round(v, 3), "m/s")

    if vitesses:

        print("vitesse moyenne =", np.mean(list(vitesses.values())), "m/s")

    if PLOT:

        afficher(temps, signaux, creux, seuils, baselines, lisses)


if __name__ == "__main__":

    main()