import express, { Request, Response } from 'express';

const app = express();
const PORT = 3000;

let ultimoEstado = 'Desconhecido';

app.use(express.json());
app.use(express.static('public'));

app.get('/status', (req: Request, res: Response) => {
  res.json({ estado: ultimoEstado });
});

app.post('/receber', (req: Request, res: Response) => {
  const { dado } = req.body;
  ultimoEstado = dado === 1 ? 'Pressionado' : 'Solto';
  console.log('Dado recebido:', ultimoEstado);
  res.send('Dados recebidos com sucesso!');
});

app.listen(PORT, () => {
  console.log(`Servidor rodando em http://localhost:${PORT}`);
});
