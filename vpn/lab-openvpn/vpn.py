#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.util import dumpNodeConnections
from mininet.node import OVSController



def run():
    "Create network and run simple performance test"
    net = Mininet(controller = OVSController)

    private_dirs = [('/etc', '/home/david/Escritorio/Seguridad-en-Redes/vpn/lab-openvpn/%(name)s/etc'), ('/var/log/', '/home/david/Escritorio/Seguridad-en-Redes/vpn/lab-openvpn/%(name)s/var/log') ]
    # Add hosts
    pc1 = net.addHost( 'pc1', ip='10.18.1.10/24', mac="00:00:00:00:00:01" )
    pc2 = net.addHost( 'pc2', ip='10.18.2.20/24', mac="00:00:00:00:00:02" )
    pc3 = net.addHost( 'pc3', privateDirs=private_dirs, ip='100.18.7.30/24', mac="00:00:00:00:00:03" )

    # Add routers
    r1 = net.addHost( 'r1', privateDirs=private_dirs, ip='10.18.1.1/24', mac="00:00:00:00:01:00" )
    r2 = net.addHost( 'r2', ip='100.18.2.2/24', mac="00:00:00:00:02:00" )
    r3 = net.addHost( 'r3', ip='100.18.3.3/24', mac="00:00:00:00:03:00" )
    r4 = net.addHost( 'r4', privateDirs=private_dirs, ip='100.18.4.4/24', mac="00:00:00:00:04:00" )
    r5 = net.addHost( 'r5', ip='100.18.6.5/24', mac="00:00:00:00:05:00" )


    # Add switches
    s1, s2, s3, s4, s5, s6, s7 = [ net.addSwitch( s ) for s in ( 's1', 's2', 's3', 's4', 's5', 's6', 's7' ) ]

    # Add controller
    c0 = net.addController( 'c0' )

    # Add links
    net.addLink( s1, pc1 )
    net.addLink( s1, r1, intfName2='r1-eth0', params2={ 'ip' : '10.18.1.1/24' } )

    net.addLink( s2, r1, intfName2='r1-eth1', params2={ 'ip' : '100.18.2.1/24' } )
    net.addLink( s2, r2, intfName2='r2-eth0', params2={ 'ip' : '100.18.2.2/24' } )


    net.addLink( s3, r2, intfName2='r2-eth1', params2={ 'ip' : '100.18.3.2/24' } )
    net.addLink( s3, r3, intfName2='r3-eth0', params2={ 'ip' : '100.18.3.3/24' } )


    net.addLink( s4, r3, intfName2='r3-eth1', params2={ 'ip' : '100.18.4.3/24' } )
    net.addLink( s4, r4, intfName2='r4-eth0', params2={ 'ip' : '100.18.4.4/24' } )

    net.addLink( s5, pc2 )
    net.addLink( s5, r4, intfName2='r4-eth1', params2={ 'ip' : '10.18.2.4/24' } )


    net.addLink( s6, r3, intfName2='r3-eth2', params2={ 'ip' : '100.18.6.3/24' } )
    net.addLink( s6, r5, intfName2='r5-eth0', params2={ 'ip' : '100.18.6.5/24' } )


    net.addLink( s7, pc3 )
    net.addLink( s7, r5, intfName2='r5-eth1', params2={ 'ip' : '100.18.7.5/24' } )




    net.start()


    # Add routes
    r1.cmd ( 'sysctl net.ipv4.ip_forward=1' )
    r2.cmd ( 'sysctl net.ipv4.ip_forward=1' )
    r3.cmd ( 'sysctl net.ipv4.ip_forward=1' )
    r4.cmd ( 'sysctl net.ipv4.ip_forward=1' )
    r5.cmd ( 'sysctl net.ipv4.ip_forward=1' )

    pc1.cmd( 'route add default gw 10.18.1.1' )
    pc2.cmd( 'route add default gw 10.18.2.4' )
    pc3.cmd( 'route add default gw 100.18.7.5' )


    r1.cmd( 'route add default gw 100.18.2.2' )
    r2.cmd( 'route add default gw 100.18.3.3' )
    r3.cmd( 'route add -net 100.18.2.0 netmask 255.255.255.0 gw 100.18.3.2' )
    r3.cmd( 'route add default gw 100.18.6.5' )
    r4.cmd( 'route add default gw 100.18.4.3' )
    r5.cmd( 'route add default gw 100.18.6.3' )

    print "Dumping host connections"
    dumpNodeConnections( net.hosts )


    CLI(net)

    net.stop()

if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
