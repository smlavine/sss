typedef struct {
    uint32_t signandinteger;
    uint32_t fraction;
} BioF32;

double bioF32(BioF32 n);
BioF32 bioToF32(double n);

const void *bioReadU8(const void *p, uint8_t *x);
const void *bioReadU8v(const void *p, size_t n, uint8_t *x);
const void *bioReadU32LE(const void *p, uint32_t *x);
const void *bioReadF32LE(const void *p, BioF32 *x);

void *bioWriteU8(void *p, uint8_t x);
void *bioWriteU8v(void *p, size_t n, const uint8_t *x);
void *bioWriteU32LE(void *p, uint32_t x);
void *bioWriteF32LE(void *p, BioF32 x);

uint8_t bioScanU8(void *f);
void bioScanU8v(void *f, size_t n, uint8_t *x);
uint32_t bioScanU32LE(void *f);
BioF32 bioScanF32LE(void *f);

void bioPrintU8(void *f, uint8_t x);
void bioPrintU8v(void *f, size_t n, const uint8_t *x);
void bioPrintU32LE(void *f, uint32_t x);
void bioPrintF32LE(void *f, BioF32 x);
