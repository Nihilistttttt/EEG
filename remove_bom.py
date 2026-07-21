"""Remove UTF-8 BOM from files. Usage: python remove_bom.py <file1> [file2] ..."""
import sys, os

def remove_bom(path):
    with open(path, 'rb') as f:
        d = f.read()
    if d[:3] == b'\xef\xbb\xbf':
        with open(path, 'wb') as f:
            f.write(d[3:])
            f.flush()
            os.fsync(f.fileno())
        print(f"BOM removed: {path}")
    else:
        print(f"No BOM: {path}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python remove_bom.py <file1> [file2] ...")
        sys.exit(1)
    for p in sys.argv[1:]:
        remove_bom(p)