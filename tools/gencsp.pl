#!/usr/bin/perl

# Generate random CSPs

# This program generalizes the generation procedure A(n, d, s), as
# described in Bernhard Nebel's article "Solving Hard Qualitative
# Temporal Reasoning Problems: Evaluating the Efficiency of Using
# the ORD-Horn Class" to general binary qualitative calculi.

# TODO: also implement H(n,d) and S(n, d, s)

# Copyright (C) 2008 Zeno Gantner
#
# This file is part of the Generic Qualitative Reasoner GQR.
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#       
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#       
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.

use strict;
use warnings;
use encoding 'utf8'; # ä

use English qw( -no_match_vars ) ; # Avoids regex performance penalty
use Carp;
use Getopt::Long;
use List::Util 'shuffle'; # Perl 5.8
use File::HomeDir;

my $data_dir = 'data';
if (-e '/usr/share/gqr/') {
    $data_dir = '/usr/share/gqr/data';
}
# Comment this out if you do not have File::HomeDir installed
my $home_dir = File::HomeDir->my_home();
if (-e "$home_dir/share/gqr/") {
    $data_dir = "$home_dir/share/gqr/data";
}
# until here
if (exists $ENV{GQR_DATA_DIR}) {
    $data_dir = $ENV{GQR_DATA_DIR};
}

# command line parameters
my $verbose                 = 0;
my $debug                   = 0;
my $calculus                = 'allen';
my $exclude_relations       = ''; # e.g. 'hornrels'
my $use_relations           = '';
my $number_of_nodes         = 0;
my $number_of_instances     = 1;
my $average_degree          = 0;
my $average_constraint_size = 0;
my $random_seed             = 0;

GetOptions(
    #'help'           => \$help,
    'verbose'             => \$verbose,
    'debug'               => \$debug,
    'calculus=s'          => \$calculus,
#    'hard'                => \$hard_problems,
    'exclude-relations=s' => \$exclude_relations,
    'use-relations=s'     => \$use_relations,
    'size=i'              => \$number_of_nodes,
    'label-size=f'        => \$average_constraint_size,
    'number=i'            => \$number_of_instances,
    'degree=f'            => \$average_degree,
    'random-seed=i'       => \$random_seed,
    'data-dir=s'          => \$data_dir,
) or croak 'Could not read command-line parameters';

my $calculus_dir = "$data_dir/$calculus/calculus/";

if ($number_of_nodes < 1) {
    print STDERR "--size must be >0\n";
    exit -1;
}
if ($average_degree <= 0) {
    print STDERR "--degree must be >0\n";
    exit -1;
}
if ($average_degree > ($number_of_nodes - 1)) {
    print STDERR "--degree must be <= --size - 1\n";
    exit -1;
}

if ($random_seed) {
    srand $random_seed;
}

print STDERR "Using data from $data_dir ...\n" if $verbose;

my @base_relations = ();

# read the converse file in order to get the relation names
open my $CONVERSE_FILE, '<',  "$calculus_dir/$calculus.conv"
    or croak "Can't open '$calculus_dir/$calculus.conv': $!";  # TODO: use English
READ_CONVERSE_FILE:
while (my $line = <$CONVERSE_FILE>) {
    next READ_CONVERSE_FILE if $line =~ /^#/;
    
    if ($line =~ /^\s*(\S+)\s+::\s+(\S+)\s*$/) {
        my $base_relation = $1;
        print STDERR "Base relation: '$base_relation'\n" if  $debug;
        push @base_relations, $base_relation;
    }
    else {
        chomp $line;
        warn "Could not read line '$line'\n";
    }
}
close $CONVERSE_FILE;

my $universal_relation = join ' ', @base_relations;
print STDERR "Universal relation: '$universal_relation'\n" if $debug;
my $number_of_base_relations = scalar @base_relations;
print STDERR "Number of base relations: $number_of_base_relations\n" if $verbose;
my $max_node_id = $number_of_nodes - 1;

if ($average_constraint_size > $number_of_base_relations) {
    print STDERR "--label-size must be <= $number_of_base_relations\n";
    exit -1;
}
if ($average_constraint_size == 0) {
    $average_constraint_size = $number_of_base_relations / 2;
}

# create --number constraint networks
foreach my $i (1 .. $number_of_instances) {
    # start of CSP description
    print "${max_node_id} #${i}-N${number_of_nodes}-R${average_constraint_size}";
    print "-D${average_degree}-S${random_seed}\n";
    
    # create graph of size --size with the universal relation on all edges
    #my @graph     = ();
    my @all_edges = ();
    foreach my $x (0 .. $max_node_id-1) {
        #$graph[$x] = [];
        foreach my $y ($x+1 .. $max_node_id) {
            #$graph[$x]->[$y] = '';
            print STDERR "$x, $y\n" if $debug;
            push @all_edges, [$x, $y];
        }
    }
    
    # select edges to instantiate
    my $number_of_edges = ($number_of_nodes * $average_degree) / 2;
    print STDERR "Instantiating $number_of_edges edges ...\n" if $verbose;
    my @shuffled_edges = shuffle(@all_edges);
    my @selected_edges = @shuffled_edges[0..$number_of_edges-1];
    if ($debug) {
        print STDERR "Selected edges:\n";
        foreach my $edge (@selected_edges) {
            my ($x, $y) = @$edge;
            print STDERR "($x, $y) ";
        }
        print STDERR "\n";
    }
    
    # select a constraint value for each selected edge
    my $probability = ($average_constraint_size - 1) / ($number_of_base_relations - 1);
    if ($probability < 0 || $probability > 1) {
        croak "Invalid probability $probability";
    }
    foreach my $edge (@selected_edges) {
        my @relations = ();
        ## select one base relation
        my $first_base_relation = $base_relations[ int(rand $number_of_base_relations) ];
        push @relations, $first_base_relation;
        ## select each of the other base relations with probability (s - 2) / #b
        BASE_RELATION:
        foreach my $base_relation (@base_relations) {
            next BASE_RELATION if $base_relation eq $first_base_relation;
            if (rand() < $probability) {
                push @relations, $base_relation;
            }
        }
        # output
        my ($x, $y) = @$edge;
        my $relations = join ' ', @relations;
        print "$x $y ($relations)\n";
    }
    
    # end of CSP description
    print ".\n";
}

