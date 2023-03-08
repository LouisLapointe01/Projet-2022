package Verif;


//bliblioth�que pour lire fichier binaire
import java.io.*;
import java.lang.*;


public class Main {
	
	/**
	 * Fonction de v�rification
	 * @param args
	 */
	public static void main(String[] args) {		
		//r�cup�ration des informations du disk
		
		RandomAccessFile lecteur;
		int nbMaxUser=5;
		
		try {
			lecteur = new RandomAccessFile("d0","r");
			
			//r�cup�ration des donn�es du super block
			
				//on a remarqu� qu'il nous fallait diviser par 2^24 affin d'obtenir la bonne valeure pour les entiers
			int error24=(int)Math.pow(2,24);
			
			int nbFichier=lecteur.readInt()/error24;
			int	nbUser=lecteur.readInt()/error24;
			int nbBlock=lecteur.readInt()/error24;
			int fFbyte=lecteur.readInt()/error24;
			
			//r�cup�ration des donn�es du catalogue			
			String[] inodeNom = new String[nbFichier];
			int[] inodeSize = new int[nbFichier];
			int[] inodeBlock = new int[nbFichier];
			int[] inodeFileFB = new int[nbFichier];
			
			lecteur.seek(17);
			
			//
			//probl�me avec les readUTF() permetant normalement de lire string
			//
			for(int i=0; i<nbFichier; i++) {
				inodeNom[i]=lecteur.readUTF();
				inodeSize[i]=lecteur.readInt()/error24;
				lecteur.seek(117+108*i);
				inodeBlock[i]=lecteur.readInt()/error24;
				inodeFileFB[i]=lecteur.readInt()/error24;
				lecteur.seek(17+108*i);
			}
			
			//r�cup�ration des donn�es de la table d'User
			String[] nomUser = new String[nbUser];
			
			nomUser[1]=lecteur.readUTF(); //la
			
			for(int i=0; i<nbMaxUser; i++) {
				nomUser[i]=lecteur.readUTF();
				lecteur.seek(1097+97*i);
			}
			
			//analyse coherence syst�me
			
			//information super block
			System.out.println("V�rification information super block :\n");
			
				//verif nb fichier
			int totalBlock=0;
			for(int i=0; i<nbFichier; i++) {
				totalBlock+=inodeBlock[i];
			}
			if(nbBlock!=totalBlock) {
				System.out.println("    1) erreur nombre de fichier incorrecte car nomre de block incorrecte");
			}
			System.out.println("    1) nombre de fichier correcte car nombre de block correcte");
			
				//verif nb User
			int totalUser=0;
			for(int i=0; i<nomUser.length; i++) {
				if(nomUser[i]!="") {
					totalUser++;
				}
			}
			if(totalUser!=nbUser) {
				System.out.println("    2) erreur total nombre d'User");
			}
			System.out.println("    2) nombre d'User correcte");
			
				//verif first free byte
			int pos=inodeFileFB[nbFichier]+inodeSize[nbFichier];
			if(pos!=fFbyte) {
				System.out.println("    3) erreur position first free byte incorecte");
			}
			
			//information table inodes pour chaque inode
			System.out.println("V�rification information Catalogue (pour cahque inodes) :\n");
			//boucle for pour verif chaque inode
			for(int i=0; i<nbFichier; i++) {
				System.out.println("  dans ino/4!=inodeBlock[i]de " + i + inodeNom[i]);
				//verif nb block
				if(inodeSize[i]/4!=inodeBlock[i]) {
					System.out.println("    1) erreur nombre de block");
				}
				System.out.println("    1) nombre de block correcte");
				
				//verif troue
				if(i+1<nbFichier) {
					if(inodeSize[i]+inodeFileFB[i]!=inodeFileFB[i+1]) {
						System.out.println("    2) pr�sence troue entre " + inodeNom[i] + " et " + inodeNom[i+1]);
					}
					System.out.println("    2) non pr�sence de troue");
				}
				else {
					System.out.println("    2) dernier fichier du disque");
				}
			}
			
			//information table d'utilisateur
			System.out.println("V�rification information Table d'utilisateur :\n");
				//   ??? ???
			
			//fermeture du lecteur
			lecteur.close();
		}
		catch (IOException ioe) {
			System.err.println(ioe);
			System.exit(2);
		}
		
		//System.out.println("testD");
	}
}
