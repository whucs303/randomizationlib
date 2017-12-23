<?php
/*the class for randomizing parameter name of very http request 
to against the CSRF Attack. 
Author:ben 
Date:2015/03/18
*/
class CSRFP_Randomization
{	
	//initial function of the class
	public static function init()
	{
	    //set the"randomkey"session
	    self::StartRandomkeySession();
	}
	//function to generate the key(32) for randomizing the parameter name
	 public static function GenerateRandomkey()
	{
	    $key="";
		for ($i=0;$i<32;$i++)
		{   //tha range of visible string ascii value:33~126 except "<","/"
		    $r=rand(33,126);
		    while($r==60||$r==47)$r=rand(33,126);
		    $key.=chr($r);
		}
		$aes256Key = hash("SHA256", $key, true);
		return $aes256Key;
	}

	public static function GenerateVi()
	{
		// for good entropy (for MCRYPT_RAND)
		srand((double) microtime() * 1000000);
		// generate random iv
		$iv = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_CBC), MCRYPT_RAND);
		return $iv;
	}
	
	//function to set session value for"randomkey"	
	public static function StartRandomkeySession()
	{
	    if(!isset($_SESSION))
		{
		    session_start();
        }
        if(!isset($_SESSION["randomkey"]))
        {
		    $_SESSION["randomkey"]=self::GenerateRandomkey();
		    $_SESSION['iv'] = self::GenerateVi();
        }
	}
	//function to randomize the parameter name with a random key
	public static function Randomization($parameter)
	{
	    if(!isset($_SESSION))
		{
		    session_start();
        }
	    if(!isset($_SESSION["randomkey"]))
        {
            $_SESSION["randomkey"]=self::GenerateRandomkey();
        }
        //joint the two string as the plaintext
	    $plainstr=$parameter.$_SESSION["randomkey"];
		//using md5 algorithm
		return md5($plainstr);
	}

	public static function Encrypt($parameter)
	{
		if(!isset($_SESSION)) 
		{	
			session_start();
		}	
		if(!isset($_SESSION['randomkey']))
		{	
			$_SESSION['randomkey'] = self::GenerateRandomkey();
			$_SESSION['iv'] = self::GenerateVi();
		}
		return rtrim(base64_encode(mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $sSecretKey, $sValue, MCRYPT_MODE_CBC, $iv)), "\0\3");
	}

	public static function Decrypt()
	{
		if(!isset($_SESSION)) 
		{	
			session_start();
		}	
		if(!isset($_SESSION['randomkey']))
		{	
			$_SESSION['randomkey'] = self::GenerateRandomkey();
			$_SESSION['iv'] = self::GenerateVi();
		}
		return rtrim(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $sSecretKey, base64_decode($sValue), MCRYPT_MODE_CBC, $iv), "\0\3");
	}
}
?>
