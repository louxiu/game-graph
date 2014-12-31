void main()
{
    gl_FragColor[0] = 0;
    gl_FragColor[1] = 0;
    gl_FragColor[2] = 1;

    if (mod(gl_FragCoord.y, 30.0) > 15){
        gl_FragColor[3] = 1;
    } else {
        gl_FragColor[3] = 0.4;
    }
}
