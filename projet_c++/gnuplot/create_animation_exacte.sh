#!/bin/bash
# ./gnuplot/create_animation.sh premiere_sortie 5
# --- PARAMÈTRES D'ENTRÉE ---
BASE_NAME="$1"       # Exemple : premiere_sortie
STEP_SKIP="$2"       # Exemple : 10 (pour ne prendre qu'une image tous les 10 pas de temps)

if [ -z "$BASE_NAME" ] || [ -z "$STEP_SKIP" ]; then
    echo "------------------------------------------------------"
    echo "Erreur: Veuillez fournir DEUX arguments."
    echo "Usage: ./gnuplot/create_animation_exacte.sh NOM_BASE K_PAS_DE_TEMPS"
    echo "Exemple (tous les 10 pas de temps): ./gnuplot/create_animation_exacte.sh premiere_sortie 10"
    echo "Exemple (tous les pas de temps): ./gnuplot/create_animation_exacte.sh premiere_sortie 1"
    echo "------------------------------------------------------"
    exit 1
fi

# Convertir STEP_SKIP en entier pour la division modulo (sécurité)
STEP_SKIP=$(echo "$STEP_SKIP" | awk '{print int($1)}')
if [ "$STEP_SKIP" -lt 1 ]; then
    echo "ERREUR: K_PAS_DE_TEMPS doit être supérieur ou égal à 1."
    exit 1
fi
# ---------------------------

INPUT_DIR="output_exacte"
OUTPUT_DIR="gnuplot/frames_exacte"
GIF_OUTPUT="gnuplot/${BASE_NAME}_exacte_animation_skip_${STEP_SKIP}.gif" 
SCRIPT_GP="gnuplot/plot_frame_exacte.gp"
SEARCH_PATTERN="${BASE_NAME}_exacte_iter_*.csv"

# 1. Préparation et Vérification
mkdir -p "$OUTPUT_DIR"
echo "--- Début de la génération pour la base: $BASE_NAME (Pas K: $STEP_SKIP) ---"
echo "Recherche des fichiers dans '$INPUT_DIR' selon le motif: '$SEARCH_PATTERN'"

# --- BLOC DE DIAGNOSTIC ---
echo "--- DIAGNOSTIC ---"
if [ ! -d "$INPUT_DIR" ]; then
    echo "ERREUR FATALE: Le dossier '$INPUT_DIR' n'existe PAS. Vérifiez le chemin."
    exit 1
fi
echo "Liste de TOUS les CSV trouvés dans '$INPUT_DIR':"
ls -l "$INPUT_DIR"/*.csv 2>/dev/null || echo "  (Aucun fichier .csv trouvé. Vérifiez le chemin et l'extension.)"
echo "Liste des fichiers trouvés par la commande 'find' (Doit lister les fichiers à traiter):"
find "$INPUT_DIR" -maxdepth 1 -type f -name "$SEARCH_PATTERN" | sort -V
echo "------------------"
# --- FIN DU BLOC DE DIAGNOSTIC ---


# 2. Boucle sur les fichiers correspondants
FRAME_COUNT=0
GLOBAL_COUNT=0 # Compteur de tous les fichiers (pour la logique de saut)

# On cherche tous les fichiers et on trie par ordre numérique (V)
while read FULL_PATH_IN; do
    
    # Vérifie si le chemin lu n'est pas vide
    if [ -z "$FULL_PATH_IN" ]; then
        continue
    fi
    
    GLOBAL_COUNT=$((GLOBAL_COUNT + 1))
    
    # Logique de saut de pas de temps (K)
    # On force le premier fichier (GLOBAL_COUNT = 1) à être inclus
    # ET on inclut les fichiers dont le numéro est divisible par STEP_SKIP
    if [ "$GLOBAL_COUNT" -ne 1 ] && [ $((GLOBAL_COUNT % STEP_SKIP)) -ne 0 ]; then
        continue # Passer au fichier suivant
    fi
    
    # -- Traitement de la Frame --
    
    FILENAME_ONLY=$(basename "$FULL_PATH_IN")
    
    # Création du nom du fichier de sortie (ex: gnuplot/frames/frame_0001.png)
    FRAME_COUNT=$((FRAME_COUNT + 1))
    FRAME_OUT="${OUTPUT_DIR}/frame__exacte$(printf "%04d" $FRAME_COUNT).png"

    echo "  Traitement du fichier #$GLOBAL_COUNT : $FILENAME_ONLY -> ${FRAME_OUT}"
    
    # Exécution de Gnuplot (le script plot_frame.gp doit être la version corrigée)
    gnuplot -c "$SCRIPT_GP" "$FULL_PATH_IN" "$FRAME_OUT"

done < <(find "$INPUT_DIR" -maxdepth 1 -type f -name "$SEARCH_PATTERN" | sort -V)

echo "--- Gnuplot a créé $FRAME_COUNT frames (sur $GLOBAL_COUNT fichiers trouvés). ---"

# ... (Partie 3: Création du GIF avec ImageMagick)


# 3. Création du GIF avec ImageMagick
if [ $FRAME_COUNT -gt 0 ]; then
    echo "--- Création du GIF animé... ---"
    convert -delay 10 -loop 0 "${OUTPUT_DIR}/frame_*.png" "$GIF_OUTPUT"
    echo "--- Succès! Le GIF est disponible ici: $GIF_OUTPUT ---"
    
    # Nettoyage des fichiers temporaires (optionnel)
    # rm -rf "$OUTPUT_DIR"
    # echo "Fichiers temporaires supprimés."
else
    echo "ERREUR: Aucun fichier trouvé pour le motif '$SEARCH_PATTERN' dans le dossier '$INPUT_DIR'."
    echo "Vérifiez : 1) que vous passez bien deux arguments ; 2) la casse ; 3) l'absence d'accents."
fi