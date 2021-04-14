# include "utils.h"
# include <cstdio>

struct pixel_prob {
	int pixel;
	float probablity;
	struct pixel_prob* left, * right;
	char code[22];
};
struct huff_code {
	int pixel;
	int arrloc;
	float probablity;
};

// function to concatenate the words 
void allc_code(char* str, char* prefix, char add){
	int i = 0;
	while (*(prefix + i) != '\0'){
		*(str + i) = *(prefix + i);
		i++;
	}
	if (add != '2'){
		str[i] = add;
		str[i + 1] = '\0';
	}
	else {
		str[i] = '\0';
	}
}
int codelen(char* code)
{
	int l = 0;
	while (*(code + l) != '\0')
		l++;
	return l;
}
Mat huffman(Mat src){
	//get histogram of total image
	float Hist[256] = { 0 };
	int tmp;
	int H = src.rows;
	int W = src.cols;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			tmp = src.at<uchar>(i, j);
			Hist[tmp] += 1;
		}
	}
	for (int i = 0; i < 256; i++) {
		Hist[i] = Hist[i] / (H * W);
	}
	//find non-zero numbur of intensity & find minimum probability except 0
	int nodes = 0;
	float min_val = 1;
	for (int i = 0; i < 256; i++) {
		if (Hist[i] != 0) {
			nodes += 1;
			if (Hist[i] < min_val) {
				min_val = Hist[i];
			}
		}
	}
	struct pixel_prob* pixel_prob;
	struct huff_code* codeword;
	int huff_nodes = 2 * nodes - 1;
	pixel_prob = (struct pixel_prob*)malloc(sizeof(struct pixel_prob) * huff_nodes);
	codeword = (struct huff_code*)malloc(sizeof(struct huff_code) * nodes);

	//initialize node
	int node = 0;
	for (int i = 0; i < 256; i++) {
		if (Hist[i] != 0) {
		//for pixel_prob struct
			pixel_prob[node].pixel = i;
			pixel_prob[node].probablity = Hist[i];
			pixel_prob[node].left = NULL;
			pixel_prob[node].right = NULL;
			pixel_prob[node].code[0] = '\0';
		//for huff_code struct
			codeword[node].pixel = i;
			codeword[node].probablity = Hist[i];
			codeword[node].arrloc = node;

			node++;
		}
	}
	// Sorting the histogram  - about probablity--selection sort
	struct huff_code tmp_huff;
	for (int i = 0; i < nodes; i++) {
		for (int j = i + 1; j < nodes; j++) {
			if (codeword[i].probablity < codeword[j].probablity) {
				tmp_huff = codeword[i];
				codeword[i] = codeword[j];
				codeword[j] = tmp_huff;
			}
		}
	}
	//generate Huffman tree
	float sum_prob;
	int sum_pixel;
	int next_node = nodes;
	int n = 0;
	while (n  < nodes - 1){
		sum_pixel = codeword[nodes - n - 1].pixel + codeword[nodes - n - 2].pixel;
		sum_prob = codeword[nodes - n - 1].probablity + codeword[nodes - n - 2].probablity;
		// gen new node
		pixel_prob[next_node].pixel = sum_pixel;
		pixel_prob[next_node].probablity = sum_prob;
		pixel_prob[next_node].left = &pixel_prob[codeword[nodes - n - 2].arrloc];
		pixel_prob[next_node].right = &pixel_prob[codeword[nodes - n - 1].arrloc];
		pixel_prob[next_node].code[0] = '\0';
		int i = 0;
		//resort
		while (sum_prob <= codeword[i].probablity) {
			i++;
		}
		for (int k = nodes; k >= 0; k--){
			if (k == i){
				codeword[k].pixel = sum_pixel;
				codeword[k].probablity = sum_prob;
				codeword[k].arrloc = next_node;
			}
			else if (k > i) {
				codeword[k] = codeword[k - 1];
			}
		}
		n += 1;
		next_node += 1;
	}
	//alloacte codeword at each node
	char left = '0';
	char right = '1';
	for (int i = huff_nodes - 1; i >= nodes; i--){
		if (pixel_prob[i].left != NULL)
			allc_code(pixel_prob[i].left->code, pixel_prob[i].code, left);
		if (pixel_prob[i].right != NULL)
			allc_code(pixel_prob[i].right->code, pixel_prob[i].code, right);
	}
	//gen compressed text file
	int pixel_value;
	int total_len = 0;
	int ori_len = 0;

	FILE* imagehuff = fopen("encoded_image.txt", "w+");
	fprintf(imagehuff, "%d\n", H);
	fprintf(imagehuff, "%d\n", W);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			pixel_value = src.at<uchar>(i, j);
			for (int k = 0; k < nodes; k++) {
				if (pixel_value == pixel_prob[k].pixel) {
					fprintf(imagehuff, "%s", pixel_prob[k].code);
					total_len += codelen(pixel_prob[k].code);
					ori_len += 8;
				}
			}

		}
	}
	fclose(imagehuff);
	//coding 
	printf("Huffmann Codes::\n\n");
	printf("pixel values -> Code\n\n");
	for (int i = 0; i < nodes; i++) {
		if (snprintf(NULL, 0, "%d", pixel_prob[i].pixel) == 2)
			printf("	 %d	 -> %s\n", pixel_prob[i].pixel, pixel_prob[i].code);
		else
			printf(" %d	 -> %s\n", pixel_prob[i].pixel, pixel_prob[i].code);
	}
	// Calculating Average Bit Length 
	float avgbitnum = 0;
	for (int i = 0; i < nodes; i++)
		avgbitnum += pixel_prob[i].probablity * codelen(pixel_prob[i].code);
	printf("Average number of bits:: %f", avgbitnum);
	printf("\noriginal len = %d", ori_len);
	printf("\ntotal = %d", total_len);

	printf("\ncompression rate = %f", 100 * (1 - (float)total_len / (float)ori_len));




	//decompress
	FILE* huffstring = fopen("encoded_image.txt", "r");
	int c;
	int i = 0;
	int j = 0;
	int root = huff_nodes-1;
	struct pixel_prob * tmp_node;
	int New_H, New_W;
	fscanf(huffstring, "%d", &New_H);
	fscanf(huffstring, "%d", &New_W);
	Mat dst(New_H, New_W, CV_8UC1);
	do{		tmp_node = &pixel_prob[root];
		do {
			c = fgetc(huffstring);
			if (c == 48) {
				tmp_node = tmp_node->left;
			}
			else if (c == 49) {
				tmp_node = tmp_node->right;
			}
		} while (tmp_node->left != NULL);
		dst.at<uchar>(i, j) = tmp_node->pixel;
		j++;
		if (j >= W) {
			i++;
			j = 0;
		}
		if (i == H) {
			break;
		}
	} while (c != -1);
	fclose(huffstring);
	return dst;
}
