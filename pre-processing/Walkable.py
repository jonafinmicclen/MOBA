from PIL import Image
from itertools import batched

MAGIC = "WALKABLEMAP"

def boolsToByte(bytes : tuple[int, ...], threshold : int) -> int:
    """
    Converts tuple of 8 ints to 1 packed byte as int
    Where each int under threshold is 0b1
    """
    if (len(bytes) != 8):
        raise Exception("Must be 8 bools to pack")
    
    packed_byte = 0
    for bit, off in zip(bytes, range(7, -1, -1)):
        val = bit < threshold
        packed_byte += int(val) << off

    return packed_byte

def convertGreyscaleToPackedBin(img : Image, threshold : int):
    """
    Converts grayscale image to packed 1bpp image.

    Pixel intensity < threshold is interpreted as 1
    """
    img_bytes = img.get_flattened_data()
    w, h = img.size

    if not (w%8==0 and h%8==0):
        raise Exception("Image dims must be multiples of 8")

    buffer_size = int(w*h/8)
    buffer = bytearray(buffer_size)

    for idx, bytes in zip(range(buffer_size), batched(img_bytes, 8)):

        packed = boolsToByte(bytes, threshold)
        buffer[idx] = packed

    return buffer

if __name__ == "__main__":
    im = Image.open("assets/Maps/MapTemplate/nav/walkable.bmp")

    buffer = convertGreyscaleToPackedBin(im, 70)

    proc_img =Image.frombuffer("1", (512, 512), buffer, )
    proc_img.save("test.png")


    with open("walkable.bin", "wb") as stream:
        stream.write(MAGIC.encode("ascii").ljust(16, b"\0"))
        stream.write(buffer)