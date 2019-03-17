from PIL import Image


def gen_bin(name):
    bit = 0
    byte = 0x00
    out = []
    rgbw = Image.open(f"{name}.png").convert("1")
    for x in range(rgbw.width):
        for y in range(rgbw.height):
            if rgbw.getpixel((x, y)) != 0:
                byte |= 1 << bit
            bit = (bit+1) % 8
            if bit == 0:
                out.append(byte)
                byte = 0x00
    assert(bit == 0)
    print('const uint8_t {} PROGMEM = {{{}}};'.format(name,
                                                      ', '.join(['0x{:02x}'.format(i) for i in out])))


gen_bin('rgbw_top')
gen_bin('r_bottom')
gen_bin('r_bottom_sel')
gen_bin('g_bottom')
gen_bin('g_bottom_sel')
gen_bin('b_bottom')
gen_bin('b_bottom_sel')
gen_bin('w_bottom')
gen_bin('w_bottom_sel')

digits = Image.open("digits.png").convert("1")
digits_bin = []
for d in range(10):
    bit = 0
    byte = 0x00
    out = []
    for col in range(11):
        for y in range(digits.height):
            x = d*13+col
            if digits.getpixel((x, y)) != 0:
                byte |= 1 << bit
            bit = (bit+1) % 8
            if bit == 0:
                out.append(byte)
                byte = 0x00
    if bit != 0:
        out.append(byte)
    digits_bin.append('{{{}}}'.format(
        ', '.join(['0x{:02x}'.format(i) for i in out])))
print('const uint8_t digits[{}][] PROGMEN = {{\n\t{},\n}};'.format(
    int((11*digits.height+7)/8), ',\n\t'.join(digits_bin)))
