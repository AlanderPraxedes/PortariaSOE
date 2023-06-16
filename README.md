# PortariaSOE
Para compilar
g++ -o placa7 placa7.cpp `pkg-config --cflags --libs opencv4` -ltesseract

g++ ProjetoSOE.cpp -o ProjetoSOE `pkg-config --cflags --libs opencv4 tesseract` -lcurl
