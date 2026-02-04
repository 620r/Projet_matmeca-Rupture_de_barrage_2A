# ----------------------------------------------------------------------
# Script Gnuplot pour tracer une seule image à partir d'un fichier CSV
# ARG1: chemin/nom_du_fichier.csv
# ARG2: nom_du_fichier_de_sortie.png
# ----------------------------------------------------------------------

file_in = ARG1
file_out = ARG2

# Paramètres généraux
set datafile separator "," # Les données sont séparées par des virgules (CSV)
set key autotitle columnhead 
set grid

# Configuration des Échelles (X et Y) fixes pour le GIF 
# AJUSTEZ CES VALEURS si elles ne correspondent pas à l'étendue réelle de vos données.
set xrange [-25:50]  # Élargissement de l'axe X (Position)
set yrange [0:30]    # Élargissement de l'axe Y (Valeurs : Hauteur, Débit, Vitesse)

# Titres
set xlabel "Position (colonne 1)"
set ylabel "Valeurs"
set title "Évolution des valeurs en fonction de la Position\n" . file_in

# Paramètres de sortie pour une image PNG
set terminal png size 800,600
set output file_out

# Commande de tracé :
# Col 1: Position (X)
# Col 2: Hauteur (Y)
# Col 3: Débit (Y)
# Col 4: Vitesse (Y)
plot file_in using 1:2 with lines title columnhead(2), \
     '' using 1:3 with lines title columnhead(3), \
     '' using 1:4 with lines title columnhead(4)

set output
set terminal pop