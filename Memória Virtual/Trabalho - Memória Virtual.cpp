#include <iostream>
#include <unordered_map>
#include <vector>

// memória física
#define F_SIZE 8

// memória virtual 
#define V_PAGES_SIZE 128
#define V_NUM_PAGES 8

// tamanho TLB

#define TLB_SIZE 4

using namespace std;

vector<int> memoriaFisica(F_SIZE, -1);
vector<pair<int, int>> TLB;
unordered_map<int, int>tabelaPaginas;

int proximoLivre = 0;

void limparChat(int i) {
    while(i--) {
        cout << endl;
    }
}

void atualizarTLB(int numeroPagina, int numeroQuadro) {
    if(TLB.size() >= TLB_SIZE) {
        TLB.erase(TLB.begin());
    }
    TLB.push_back({numeroPagina, numeroQuadro});
}

int procuraNaFisica(int numeroPagina, int offset) {
    if(proximoLivre >= F_SIZE) {
        cout << "Nao foi possível encontrar na memoria fisica!" << endl;
        cout << "Memoria física excedida!" << endl;
        exit(0);
    }
    cout << "Carregando pagina: " << numeroPagina << " offset: " << offset << endl;
    tabelaPaginas[numeroPagina] = proximoLivre;
    memoriaFisica[proximoLivre] = numeroPagina;
    atualizarTLB(numeroPagina, proximoLivre);
    proximoLivre++;
    return ((proximoLivre+1) * numeroPagina * V_PAGES_SIZE) + offset + 256;
}

int traduzirEndereco(int virtualAdress) {
    int numeroPagina = virtualAdress / V_PAGES_SIZE;
    int offset = virtualAdress % V_PAGES_SIZE;

    // buscando na TLB:
    for(const auto &entry : TLB) {
        if(entry.first == numeroPagina) {
            cout << "Pagina encontrada (TLB HIT): " << numeroPagina << " offset: " << offset << endl;
            return ((entry.second + 1) * numeroPagina * V_PAGES_SIZE) + offset + 256;
        }
    }
    cout << "Endereco nao encontrado na TLB. Buscando na memoria virtual ->" << endl;

    // buscando na memória virtual. 

    if(tabelaPaginas.find(numeroPagina) != tabelaPaginas.end()) {
        cout << "Pagina encontrada (MV HIT): " << numeroPagina << " offset: " << offset << endl;

        // adicionar na TLB como último usado.

        atualizarTLB(numeroPagina, tabelaPaginas.find(numeroPagina)->second);     
        return ((tabelaPaginas.find(numeroPagina)->second+1) * numeroPagina * V_PAGES_SIZE) + offset + 256;
    }
    cout << "Pagina nao encontrada na MV... Loading page..." << endl;
    return procuraNaFisica(numeroPagina, offset);
}

void result(int address) {
    cout << "\n" << endl;
    cout << "O endereco solicitado eh: 0v" << address << endl;
    cout << "\n" << endl;
    address = traduzirEndereco(address);
    cout << "\n" << endl;
    cout << "----------------------------------" << endl;
    cout << "O endereco fisico eh: 0xF" << address << endl;
}

int main() {
    int enderecosVirtuais[5] = {1, 1023, 1000, 564, 1024};
    int s = 0, n = 0; int address = 0;
    for(int i = 0; i < 5; i++) {
        traduzirEndereco(enderecosVirtuais[i]); // pre-load para testes futuros.
    }
    limparChat(20);
    while(s != -1) {
        cout << "Digite alguma opcao para prosseguir:\n1 - Solicitar um endereco virtual\n2 - Mostrar TLB\n3 - Mostrar MV\n-1 - Sair";
        cout << endl;
        cin >> n;
        switch(n){
            case 1:
                cout << "Digite um endereco para buscar: " << endl;
                cin >> address;
                while(address <= 0 || address > 1024) {
                    cout << "Digite um endereco para buscar: " << endl;
                    cin >> address;
                }
                result(address);
                cout << endl;
                break;
            case 2:
                cout << "---------------------------" << endl;
                for(const auto &entry : TLB) {
                    cout << "Pagina: " << entry.first << " esta associada ao bloco fisico: " << entry.second << endl;
                }
                break;
            case 3: 
                for(const auto &entry : tabelaPaginas) {
                    cout << "[ MV ]Pagina: " << entry.first << " bloco fisico: " << entry.second << endl;       
                }
                break;
            default:
                s = -1;      
        }
        n = 0;
    }
    return 0;
}
