#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const bool DEAD = false;
const bool ALIVE = true;

const bool nextCell[2][9] = {
	[DEAD] = {
		[0] = DEAD,
		[1] = DEAD,
		[2] = DEAD,
		[3] = ALIVE,
		[4] = DEAD,
		[5] = DEAD,
		[6] = DEAD,
		[7] = DEAD,
		[8] = DEAD
	},
	[ALIVE] = {
		[0] = DEAD,
		[1] = DEAD,
		[2] = ALIVE,
		[3] = ALIVE,
		[4] = DEAD,
		[5] = DEAD,
		[6] = DEAD,
		[7] = DEAD,
		[8] = DEAD
	}
};

struct Grid {
	bool *cells;
	int w;
	int h;
};

bool gridRead(Grid *g, FILE *f) {
	char line[1024];

	for (int y = 0; y < g->h; y++) {
		fgets(line, sizeof(line), f);
		if (strlen(line) != g->w + 1) {
			return false;
		}
		for (int x = 0; x < g->w; x++) {
			switch (line[x]) {
			case '0':
				g->cells[y * g->w + x] = DEAD;
				break;
			case '1':
				g->cells[y * g->w + x] = ALIVE;
				break;
			default:
				return false;
			}
		}
	}

	return true;
}

Grid *gridFromFile(const char *path) {
	FILE *f;
	char line[1024];
	Grid *g;
	int w;
	int h;

	f = fopen(path, "r");
	if (!f) {
		return NULL;
	}

	fgets(line, sizeof(line), f);
	w = atoi(line);
	fgets(line, sizeof(line), f);
	h = atoi(line);

	if (w <= 0 || h <= 0 || w > 100 || h > 100) {
		fclose(f);
		return NULL;
	}

	g = (Grid *)malloc(sizeof(Grid));
	g->cells = (bool *)calloc(w * h, sizeof(bool));
	g->w = w;
	g->h = h;

	if (!gridRead(g, f)) {
		fclose(f);
		free(g->cells);
		free(g);
		return NULL;
	}

	fclose(f);
	return g;
}

void gridDestroy(Grid *g) {
	free(g->cells);
}

Grid *gridClone(Grid *g) {
	Grid *h;

	h = (Grid *)malloc(sizeof(Grid));
	h->cells = (bool *)calloc(g->w * g->h, sizeof(bool));
	h->w = g->w;
	h->h = g->h;

	return h;
}

void gridPrint(Grid *g) {
	char out[(g->w + 3) * (g->h + 2) + 1];
	int n = 0;

	out[n++] = '+';
	for (int x = 0; x < g->w; x++) {
		out[n++] = '-';
	}
	out[n++] = '+';
	out[n++] = '\n';
	for (int y = 0; y < g->h; y++) {
		out[n++] = '|';
		for (int x = 0; x < g->w; x++) {
			bool alive = g->cells[y * g->h + x];
			out[n++] = alive ? 'o' : ' ';
		}
		out[n++] = '|';
		out[n++] = '\n';
	}
	out[n++] = '+';
	for (int x = 0; x < g->w; x++) {
		out[n++] = '-';
	}
	out[n++] = '+';
	out[n++] = '\n';

	out[n++] = '\0';

	printf("%s", out);
}

bool neighbor(Grid *g, int x, int y) {
	x = (x + g->w) % g->w;
	y = (y + g->h) % g->h;
	return g->cells[y * g->w + x];
}

int neighbors(Grid *g, int x, int y) {
	int w = g->w;
	int h = g->h;
	int n = 0;

	if (neighbor(g, x - 1, y - 1)) {
		n += 1;
	}
	if (neighbor(g, x, y - 1)) {
		n += 1;
	}
	if (neighbor(g, x + 1, y - 1)) {
		n += 1;
	}
	if (neighbor(g, x - 1, y)) {
		n += 1;
	}
	if (neighbor(g, x + 1, y)) {
		n += 1;
	}
	if (neighbor(g, x - 1, y + 1)) {
		n += 1;
	}
	if (neighbor(g, x, y + 1)) {
		n += 1;
	}
	if (neighbor(g, x + 1, y + 1)) {
		n += 1;
	}

	return n;
}

Grid *generation(Grid *prev) {
	Grid *next = gridClone(prev);
	int w = prev->w;
	int h = prev->h;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			bool c = prev->cells[y * w + x];
			int n = neighbors(prev, x, y);
			next->cells[y * w + x] = nextCell[c][n];
		}
	}

	return next;
}

int main() {
	Grid *g = gridFromFile("board.txt");
	if (!g) {
		fprintf(stderr, "Could not load board.txt\n");
		return 1;
	}
	gridPrint(g);

	for (int i = 0; i < 100; i++) {
		usleep(100 * 1000);
		Grid *next = generation(g);
		gridDestroy(g);
		g = next;
		gridPrint(g);
	}

	gridDestroy(g);
	return 0;
}
