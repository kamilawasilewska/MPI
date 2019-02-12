file = fopen('result1500_1200.bin');
data = fread(file);
fclose(file);
data = reshape(data, [1500 1200]);

imshow(data, [0 255]);