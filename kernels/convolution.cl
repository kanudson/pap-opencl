

__kernel void convolve(
                       const __global uint* const input,
                       __constant uint* const mask,
                       __global   uint* const output,
                       const int inputWidth,
                       const int maskWidth)
{
    //  current X/Y Position in output image
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    uint sum = 0;
    for (int posy = 0; posy < maskWidth; ++posy)
    {
        const int idxInTemp = (y + posy) * inputWidth + x;

        for (int posx = 0; posx < maskWidth; ++posx)
        {
            sum = mask[(posy * maskWidth) + posx] * input[idxInTemp + posx];
        }
    }

    output[y * get_global_size(0) + x] = sum;
}

