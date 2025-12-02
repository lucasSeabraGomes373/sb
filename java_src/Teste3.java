public class Teste3 {
    public static int somar(int a, int b) {
        return a + b;
    }
    
    public static int multiplicar(int a, int b) {
        return a * b;
    }
    
    public static int fibonacci(int n) {
        if (n <= 1) return n;
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
    
    public static void main(String[] args) {
        // Test basic arithmetic
        int x = 15;
        int y = 8;
        int soma = somar(x, y);
        int produto = multiplicar(x, y);
        
        System.out.println(soma);      // 23
        System.out.println(produto);   // 120
        
        // Test conditionals and loops
        int contador = 0;
        int i = 1;
        while (i <= 5) {
            contador = contador + i;
            i = i + 1;
        }
        System.out.println(contador);  // 15 (1+2+3+4+5)
        
        // Test fibonacci
        int fib = fibonacci(6);
        System.out.println(fib);       // 8
        
        // Test array access (if supported)
        int[] numeros = new int[3];
        numeros[0] = 10;
        numeros[1] = 20;
        numeros[2] = 30;
        System.out.println(numeros[0]);
        System.out.println(numeros[1]);
        System.out.println(numeros[2]);
    }
}
