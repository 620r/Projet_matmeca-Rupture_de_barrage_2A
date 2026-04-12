"""
Détecteur de front de vague et calculateur de vitesse
======================================================
Canal à houle – rupture de barrage
3 capteurs espacés de 15 cm, données en Volt.

Dépendances : numpy, scipy, matplotlib  (PAS de pandas)

Usage :
    python wave_speed_detector.py chemin/vers/fichier.d7d
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

# ─────────────────────────────────────────────────
# PARAMÈTRES À AJUSTER
# ─────────────────────────────────────────────────
FICHIER          = "kastor_choco_3.txt" 
DISTANCE_CM      = 15.0
DISTANCE_M       = DISTANCE_CM / 100.0

SEUIL_RELATIF    = 0.05   # 5 % de l'amplitude totale
FENETRE_BASELINE = 1000   # points pour estimer le niveau de repos
SMOOTH_WINDOW    = 51     # fenêtre Savitzky-Golay (impair)
SMOOTH_POLY      = 3

PLOT             = False
# ─────────────────────────────────────────────────


def lire_fichier(path):
    """Lit le fichier .d7d sans pandas. Retourne 4 arrays numpy."""
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

    return (np.array(t_lst), np.array(a0_lst),
            np.array(a1_lst), np.array(a2_lst))


def lisser(signal):
    w = SMOOTH_WINDOW if SMOOTH_WINDOW % 2 == 1 else SMOOTH_WINDOW + 1
    w = min(w, len(signal) - 1 if len(signal) % 2 == 0 else len(signal))
    return savgol_filter(signal, window_length=w, polyorder=SMOOTH_POLY)


def detecter_front(temps, signal, label=""):
    sig_lisse = lisser(signal)
    baseline  = np.median(sig_lisse[:FENETRE_BASELINE])
    amplitude = np.max(sig_lisse) - baseline

    if amplitude < 1e-6:
        print(f"  [{label}] Signal quasi-constant, impossible de détecter un front.")
        return None

    seuil   = baseline + SEUIL_RELATIF * amplitude
    indices = np.where(sig_lisse > seuil)[0]

    if len(indices) == 0:
        print(f"  [{label}] Aucun front détecté (seuil={seuil:.4f} V).")
        return None

    idx     = indices[0]
    t_front = temps[idx]
    print(f"  [{label}] Baseline={baseline:.4f} V | Seuil={seuil:.4f} V | "
          f"Front à t={t_front:.5f} s (indice {idx})")
    return t_front, idx, seuil, baseline, sig_lisse


def calculer_vitesse(t1, t2, t3):
    res = {}
    if t1 is not None and t2 is not None and (t2 - t1) > 0:
        res["v_01"] = DISTANCE_M / (t2 - t1)
    if t2 is not None and t3 is not None and (t3 - t2) > 0:
        res["v_12"] = DISTANCE_M / (t3 - t2)
    if t1 is not None and t3 is not None and (t3 - t1) > 0:
        res["v_02"] = (2 * DISTANCE_M) / (t3 - t1)
    return res


def afficher(temps, signaux_bruts, fronts, seuils, baselines, sig_lisses):
    labels   = ["AI0", "AI1", "AI2"]
    couleurs = ["tab:blue", "tab:orange", "tab:green"]

    fig, axes = plt.subplots(3, 1, figsize=(12, 9), sharex=True)
    fig.suptitle("Détection des fronts de vague – Premier passage", fontsize=14)

    for i, ax in enumerate(axes):
        ax.plot(temps, signaux_bruts[i], color=couleurs[i],
                alpha=0.3, linewidth=0.6, label="Brut")
        ax.plot(temps, sig_lisses[i], color=couleurs[i],
                linewidth=1.2, label="Lissé")
        ax.axhline(seuils[i],    color="red",  linestyle="--",
                   linewidth=1, label=f"Seuil ({seuils[i]:.3f} V)")
        ax.axhline(baselines[i], color="gray", linestyle=":",
                   linewidth=1, label=f"Baseline ({baselines[i]:.3f} V)")
        if fronts[i] is not None:
            ax.axvline(fronts[i][0], color="black", linewidth=1.5,
                       label=f"Front t={fronts[i][0]:.4f} s")
        ax.set_ylabel(f"{labels[i]} [V]")
        ax.legend(fontsize=7, loc="upper right")
        ax.grid(True, alpha=0.3)

    axes[-1].set_xlabel("Temps [s]")
    plt.tight_layout()
    plt.savefig("fronts_vague.png", dpi=150)
    print("\nGraphique sauvegardé : fronts_vague.png")
    plt.show()


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else FICHIER
    print(f"Lecture de : {path}")

    temps, ai0, ai1, ai2 = lire_fichier(path)
    signaux = [ai0, ai1, ai2]
    labels  = ["AI0", "AI1", "AI2"]

    dt = temps[1] - temps[0]
    print(f"  -> {len(temps)} points | dt={dt:.5f} s | durée={temps[-1]:.3f} s\n")

    fronts     = []
    seuils     = []
    baselines  = []
    sig_lisses = []

    for sig, lab in zip(signaux, labels):
        res = detecter_front(temps, sig, label=lab)
        if res:
            t_f, idx, seuil, baseline, sl = res
            fronts.append((t_f, idx))
            seuils.append(seuil)
            baselines.append(baseline)
            sig_lisses.append(sl)
        else:
            fronts.append(None)
            baseline = float(np.median(sig[:FENETRE_BASELINE]))
            sl = lisser(sig)
            seuils.append(baseline)
            baselines.append(baseline)
            sig_lisses.append(sl)

    t1 = fronts[0][0] if fronts[0] else None
    t2 = fronts[1][0] if fronts[1] else None
    t3 = fronts[2][0] if fronts[2] else None

    print("\n--- Résultats de vitesse -------------------------------------------")
    vitesses = calculer_vitesse(t1, t2, t3)

    if "v_01" in vitesses:
        print(f"  Capteur 0 -> 1  :  dt = {t2-t1:.5f} s  ->  V = {vitesses['v_01']:.3f} m/s")
    if "v_12" in vitesses:
        print(f"  Capteur 1 -> 2  :  dt = {t3-t2:.5f} s  ->  V = {vitesses['v_12']:.3f} m/s")
    if "v_02" in vitesses:
        print(f"  Capteur 0 -> 2  :  dt = {t3-t1:.5f} s  ->  V = {vitesses['v_02']:.3f} m/s  (sur 30 cm)")

    vals = list(vitesses.values())
    if vals:
        print(f"\n  ★ Vitesse moyenne : {np.mean(vals):.3f} m/s")
        print("Détails vitesse :", vals)
    else:
        print("  Impossible de calculer la vitesse (fronts manquants).")
    print("--------------------------------------------------------------------")

    if PLOT:
        afficher(temps, signaux, fronts, seuils, baselines, sig_lisses)


if __name__ == "__main__":
    main()